#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QUrl>
#include <QDateTime>
#include "CommonTypes.h"

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    bool initialize();
    void close();
    
    // Security events
    void logSecurityEvent(const SecurityEvent &event);
    QList<SecurityEvent> getSecurityEvents(int limit = 100) const;
    
    // Browsing history
    void logBrowsingHistory(const QString &profileId, const QUrl &url, 
                           const QString &title, const QString &safetyStatus);
    QList<QPair<QUrl, QString>> getBrowsingHistory(const QString &profileId, int limit = 100) const;
    
    // Settings
    void setSetting(const QString &key, const QString &value);
    QString getSetting(const QString &key, const QString &defaultValue = QString()) const;

private:
    QSqlDatabase m_database;
    bool createTables();
    bool runMigrations();
};

#endif // DATABASEMANAGER_H