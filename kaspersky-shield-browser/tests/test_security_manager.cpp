#include <QtTest>
#include <QUrl>
#include "SecurityManager.h"

class SecurityManagerTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testUrlSafetyClassification();
    void testDangerousUrlDetection();
    void testSafeUrlDetection();
    void testAsyncUrlCheck();

private:
    SecurityManager *m_securityManager;
};

void SecurityManagerTest::initTestCase()
{
    m_securityManager = new SecurityManager(this);
}

void SecurityManagerTest::cleanupTestCase()
{
    delete m_securityManager;
}

void SecurityManagerTest::testUrlSafetyClassification()
{
    // Test safe URLs
    QUrl safeUrl("https://www.kaspersky.com");
    UrlCheckResult result = m_securityManager->checkUrl(safeUrl);
    QCOMPARE(result.safety, UrlSafety::Safe);
    QVERIFY(!result.shouldBlock);
    
    // Test Google
    QUrl googleUrl("https://www.google.com");
    result = m_securityManager->checkUrl(googleUrl);
    QCOMPARE(result.safety, UrlSafety::Safe);
    QVERIFY(!result.shouldBlock);
}

void SecurityManagerTest::testDangerousUrlDetection()
{
    // Test phishing URL
    QUrl phishingUrl("https://phishing-site.com");
    UrlCheckResult result = m_securityManager->checkUrl(phishingUrl);
    QCOMPARE(result.safety, UrlSafety::Dangerous);
    QVERIFY(result.shouldBlock);
    
    // Test malware URL
    QUrl malwareUrl("https://malware-distribution.com");
    result = m_securityManager->checkUrl(malwareUrl);
    QCOMPARE(result.safety, UrlSafety::Dangerous);
    QVERIFY(result.shouldBlock);
}

void SecurityManagerTest::testSafeUrlDetection()
{
    // Test Microsoft
    QUrl microsoftUrl("https://www.microsoft.com");
    UrlCheckResult result = m_securityManager->checkUrl(microsoftUrl);
    QCOMPARE(result.safety, UrlSafety::Safe);
    QVERIFY(!result.shouldBlock);
    
    // Test Apple
    QUrl appleUrl("https://www.apple.com");
    result = m_securityManager->checkUrl(appleUrl);
    QCOMPARE(result.safety, UrlSafety::Safe);
    QVERIFY(!result.shouldBlock);
}

void SecurityManagerTest::testAsyncUrlCheck()
{
    QUrl testUrl("https://www.example.com");
    
    // Connect to the signal
    QSignalSpy spy(m_securityManager, &SecurityManager::urlCheckCompleted);
    
    // Start async check
    m_securityManager->checkUrlAsync(testUrl);
    
    // Wait for signal (with timeout)
    QVERIFY(spy.wait(5000));
    
    // Verify signal was emitted
    QCOMPARE(spy.count(), 1);
    
    // Get the result
    QList<QVariant> arguments = spy.takeFirst();
    QUrl emittedUrl = arguments.at(0).toUrl();
    UrlCheckResult result = arguments.at(1).value<UrlCheckResult>();
    
    QCOMPARE(emittedUrl, testUrl);
    QVERIFY(!result.shouldBlock); // Example.com should be safe
}

QTEST_MAIN(SecurityManagerTest)