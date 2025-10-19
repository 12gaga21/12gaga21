#include "DatabaseManager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
}

DatabaseManager::~DatabaseManager()
{
    close();
}

bool DatabaseManager::initialize()
{
    // Get data directory
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    
    // Set up database
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(dataDir + "/kaspersky_shield.db");
    
    if (!m_database.open()) {
        qCritical() << "Cannot open database:" << m_database.lastError();
        return false;
    }
    
    qDebug() << "Database opened successfully:" << m_database.databaseName();
    
    return createTables() && runMigrations();
}

void DatabaseManager::close()
{
    if (m_database.isOpen()) {
        m_database.close();
        qDebug() << "Database closed";
    }
}

bool DatabaseManager::createTables()
{
    QSqlQuery query(m_database);
    
    // Settings table
    if (!query.exec("CREATE TABLE IF NOT EXISTS settings ("
                   "key TEXT PRIMARY KEY,"
                   "value TEXT NOT NULL,"
                   "updated_at DATETIME DEFAULT CURRENT_TIMESTAMP"
                   ")")) {
        qCritical() << "Failed to create settings table:" << query.lastError();
        return false;
    }
    
    // Profiles table
    if (!query.exec("CREATE TABLE IF NOT EXISTS profiles ("
                   "id TEXT PRIMARY KEY,"
                   "name TEXT NOT NULL,"
                   "profile_type TEXT CHECK(profile_type IN ('parent', 'child', 'teen')),"
                   "time_limit_minutes INTEGER DEFAULT 120,"
                   "bed_time_start TIME,"
                   "bed_time_end TIME,"
                   "current_usage_minutes INTEGER DEFAULT 0,"
                   "last_reset_date DATE,"
                   "is_active BOOLEAN DEFAULT TRUE,"
                   "created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
                   ")")) {
        qCritical() << "Failed to create profiles table:" << query.lastError();
        return false;
    }
    
    // Browsing history table
    if (!query.exec("CREATE TABLE IF NOT EXISTS browsing_history ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "profile_id TEXT,"
                   "url TEXT NOT NULL,"
                   "title TEXT,"
                   "safety_status TEXT,"
                   "visit_time DATETIME DEFAULT CURRENT_TIMESTAMP,"
                   "FOREIGN KEY(profile_id) REFERENCES profiles(id)"
                   ")")) {
        qCritical() << "Failed to create browsing_history table:" << query.lastError();
        return false;
    }
    
    // Security events table
    if (!query.exec("CREATE TABLE IF NOT EXISTS security_events ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "type TEXT NOT NULL,"
                   "severity TEXT NOT NULL,"
                   "details TEXT,"
                   "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP"
                   ")")) {
        qCritical() << "Failed to create security_events table:" << query.lastError();
        return false;
    }
    
    // Blocked attempts table
    if (!query.exec("CREATE TABLE IF NOT EXISTS blocked_attempts ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "profile_id TEXT,"
                   "url TEXT NOT NULL,"
                   "reason TEXT,"
                   "category TEXT,"
                   "block_time DATETIME DEFAULT CURRENT_TIMESTAMP,"
                   "FOREIGN KEY(profile_id) REFERENCES profiles(id)"
                   ")")) {
        qCritical() << "Failed to create blocked_attempts table:" << query.lastError();
        return false;
    }
    
    // Create indexes
    query.exec("CREATE INDEX IF NOT EXISTS idx_history_profile_time ON browsing_history(profile_id, visit_time)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_blocked_profile_time ON blocked_attempts(profile_id, block_time)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_security_events_time ON security_events(timestamp)");
    
    qDebug() << "Database tables created successfully";
    return true;
}

bool DatabaseManager::runMigrations()
{
    // This is where database migrations would be run
    // For now, we'll just return true
    return true;
}

void DatabaseManager::logSecurityEvent(const SecurityEvent &event)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO security_events (type, severity, details, timestamp) VALUES (?, ?, ?, ?)");
    query.addBindValue(event.type);
    query.addBindValue(event.severity);
    query.addBindValue(event.details);
    query.addBindValue(event.timestamp);
    
    if (!query.exec()) {
        qWarning() << "Failed to log security event:" << query.lastError();
    } else {
        qDebug() << "Security event logged:" << event.type;
    }
}

QList<SecurityEvent> DatabaseManager::getSecurityEvents(int limit) const
{
    QList<SecurityEvent> events;
    QSqlQuery query(m_database);
    query.prepare("SELECT type, severity, details, timestamp FROM security_events "
                  "ORDER BY timestamp DESC LIMIT ?");
    query.addBindValue(limit);
    
    if (query.exec()) {
        while (query.next()) {
            SecurityEvent event;
            event.type = query.value(0).toString();
            event.severity = query.value(1).toString();
            event.details = query.value(2).toString();
            event.timestamp = query.value(3).toDateTime();
            events.append(event);
        }
    } else {
        qWarning() << "Failed to get security events:" << query.lastError();
    }
    
    return events;
}

void DatabaseManager::logBrowsingHistory(const QString &profileId, const QUrl &url, 
                                        const QString &title, const QString &safetyStatus)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO browsing_history (profile_id, url, title, safety_status) VALUES (?, ?, ?, ?)");
    query.addBindValue(profileId);
    query.addBindValue(url.toString());
    query.addBindValue(title);
    query.addBindValue(safetyStatus);
    
    if (!query.exec()) {
        qWarning() << "Failed to log browsing history:" << query.lastError();
    }
}

QList<QPair<QUrl, QString>> DatabaseManager::getBrowsingHistory(const QString &profileId, int limit) const
{
    QList<QPair<QUrl, QString>> history;
    QSqlQuery query(m_database);
    query.prepare("SELECT url, title FROM browsing_history WHERE profile_id = ? "
                  "ORDER BY visit_time DESC LIMIT ?");
    query.addBindValue(profileId);
    query.addBindValue(limit);
    
    if (query.exec()) {
        while (query.next()) {
            QUrl url = QUrl(query.value(0).toString());
            QString title = query.value(1).toString();
            history.append(qMakePair(url, title));
        }
    } else {
        qWarning() << "Failed to get browsing history:" << query.lastError();
    }
    
    return history;
}

void DatabaseManager::setSetting(const QString &key, const QString &value)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT OR REPLACE INTO settings (key, value, updated_at) VALUES (?, ?, CURRENT_TIMESTAMP)");
    query.addBindValue(key);
    query.addBindValue(value);
    
    if (!query.exec()) {
        qWarning() << "Failed to set setting:" << query.lastError();
    }
}

QString DatabaseManager::getSetting(const QString &key, const QString &defaultValue) const
{
    QSqlQuery query(m_database);
    query.prepare("SELECT value FROM settings WHERE key = ?");
    query.addBindValue(key);
    
    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }
    
    return defaultValue;
}