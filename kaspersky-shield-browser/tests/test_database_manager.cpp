#include <QtTest>
#include <QUrl>
#include <QDateTime>
#include "DatabaseManager.h"

class DatabaseManagerTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testDatabaseInitialization();
    void testSecurityEventLogging();
    void testBrowsingHistoryLogging();
    void testSettingsStorage();

private:
    DatabaseManager *m_databaseManager;
};

void DatabaseManagerTest::initTestCase()
{
    m_databaseManager = new DatabaseManager(this);
    QVERIFY(m_databaseManager->initialize());
}

void DatabaseManagerTest::cleanupTestCase()
{
    m_databaseManager->close();
    delete m_databaseManager;
}

void DatabaseManagerTest::testDatabaseInitialization()
{
    // Test that database was initialized successfully
    QVERIFY(m_databaseManager != nullptr);
}

void DatabaseManagerTest::testSecurityEventLogging()
{
    // Test logging a security event
    SecurityEvent event;
    event.type = "URL_BLOCKED";
    event.severity = "HIGH";
    event.details = "Malicious URL blocked";
    event.timestamp = QDateTime::currentDateTime();
    
    // This should not crash
    m_databaseManager->logSecurityEvent(event);
    
    // Test retrieving security events
    QList<SecurityEvent> events = m_databaseManager->getSecurityEvents(10);
    QVERIFY(events.size() >= 1);
    
    // Check that our event was logged
    bool found = false;
    for (const SecurityEvent &e : events) {
        if (e.type == "URL_BLOCKED" && e.severity == "HIGH") {
            found = true;
            break;
        }
    }
    QVERIFY(found);
}

void DatabaseManagerTest::testBrowsingHistoryLogging()
{
    // Test logging browsing history
    QUrl testUrl("https://www.kaspersky.com");
    QString title = "Kaspersky Lab";
    QString safetyStatus = "SAFE";
    
    m_databaseManager->logBrowsingHistory("parent", testUrl, title, safetyStatus);
    
    // Test retrieving browsing history
    QList<QPair<QUrl, QString>> history = m_databaseManager->getBrowsingHistory("parent", 10);
    QVERIFY(history.size() >= 1);
    
    // Check that our entry was logged
    bool found = false;
    for (const auto &entry : history) {
        if (entry.first == testUrl && entry.second == title) {
            found = true;
            break;
        }
    }
    QVERIFY(found);
}

void DatabaseManagerTest::testSettingsStorage()
{
    // Test setting and getting a setting
    QString key = "test_setting";
    QString value = "test_value";
    
    m_databaseManager->setSetting(key, value);
    
    QString retrievedValue = m_databaseManager->getSetting(key);
    QCOMPARE(retrievedValue, value);
    
    // Test default value
    QString defaultValue = m_databaseManager->getSetting("non_existent_key", "default");
    QCOMPARE(defaultValue, QString("default"));
}

QTEST_MAIN(DatabaseManagerTest)