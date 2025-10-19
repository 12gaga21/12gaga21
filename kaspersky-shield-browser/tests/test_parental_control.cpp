#include <QtTest>
#include <QUrl>
#include "ParentalControlManager.h"

class ParentalControlTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testProfileCreation();
    void testUrlFiltering();
    void testTimeLimits();
    void testBedTimeRestrictions();

private:
    ParentalControlManager *m_parentalControl;
};

void ParentalControlTest::initTestCase()
{
    m_parentalControl = new ParentalControlManager(this);
}

void ParentalControlTest::cleanupTestCase()
{
    delete m_parentalControl;
}

void ParentalControlTest::testProfileCreation()
{
    // Test creating a child profile
    UserProfile childProfile;
    childProfile.id = "child1";
    childProfile.name = "Child User";
    childProfile.type = ProfileType::Child;
    childProfile.timeLimitMinutes = 60;
    childProfile.isActive = true;
    
    QVERIFY(m_parentalControl->createProfile(childProfile));
    
    // Test getting the profile
    UserProfile retrieved = m_parentalControl->getProfile("child1");
    QCOMPARE(retrieved.name, QString("Child User"));
    QCOMPARE(retrieved.type, ProfileType::Child);
    QCOMPARE(retrieved.timeLimitMinutes, 60);
}

void ParentalControlTest::testUrlFiltering()
{
    // Test that adult content is blocked for child profiles
    QUrl adultUrl("https://adult-content.com");
    bool isAllowed = m_parentalControl->isUrlAllowed(adultUrl, "child1");
    QVERIFY(!isAllowed);
    
    // Test that safe content is allowed
    QUrl safeUrl("https://www.kaspersky.com");
    isAllowed = m_parentalControl->isUrlAllowed(safeUrl, "child1");
    QVERIFY(isAllowed);
}

void ParentalControlTest::testTimeLimits()
{
    // Test time limit enforcement
    UserProfile profile = m_parentalControl->getProfile("child1");
    QCOMPARE(profile.timeLimitMinutes, 60);
    
    // Test that time is allowed initially
    bool timeAllowed = m_parentalControl->isTimeAllowed("child1");
    QVERIFY(timeAllowed);
}

void ParentalControlTest::testBedTimeRestrictions()
{
    // Test bedtime restrictions
    UserProfile profile = m_parentalControl->getProfile("child1");
    profile.bedTimeStart = QTime(21, 0); // 9 PM
    profile.bedTimeEnd = QTime(7, 0);    // 7 AM
    m_parentalControl->updateProfile(profile);
    
    // Test that bedtime restrictions are respected
    // (This would need to be tested with specific times)
    bool timeAllowed = m_parentalControl->isTimeAllowed("child1");
    QVERIFY(timeAllowed); // Should be allowed during non-bedtime hours
}

QTEST_MAIN(ParentalControlTest)