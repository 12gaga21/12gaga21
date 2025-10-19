#include <QtTest>
#include <QTime>
#include "ProfileManager.h"

class ProfileManagerTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testProfileCreation();
    void testProfileUpdate();
    void testProfileDeletion();
    void testCurrentProfileSwitching();
    void testProfileSwitching();

private:
    ProfileManager *m_profileManager;
};

void ProfileManagerTest::initTestCase()
{
    m_profileManager = new ProfileManager(this);
}

void ProfileManagerTest::cleanupTestCase()
{
    delete m_profileManager;
}

void ProfileManagerTest::testProfileCreation()
{
    // Test creating a new profile
    UserProfile profile;
    profile.id = "test_user";
    profile.name = "Test User";
    profile.type = ProfileType::Child;
    profile.timeLimitMinutes = 120;
    profile.isActive = true;
    
    QVERIFY(m_profileManager->createProfile(profile));
    
    // Test that profile was created
    UserProfile retrieved = m_profileManager->getProfile("test_user");
    QCOMPARE(retrieved.name, QString("Test User"));
    QCOMPARE(retrieved.type, ProfileType::Child);
    QCOMPARE(retrieved.timeLimitMinutes, 120);
}

void ProfileManagerTest::testProfileUpdate()
{
    // Test updating an existing profile
    UserProfile profile = m_profileManager->getProfile("test_user");
    profile.name = "Updated Test User";
    profile.timeLimitMinutes = 180;
    
    QVERIFY(m_profileManager->updateProfile(profile));
    
    // Test that profile was updated
    UserProfile retrieved = m_profileManager->getProfile("test_user");
    QCOMPARE(retrieved.name, QString("Updated Test User"));
    QCOMPARE(retrieved.timeLimitMinutes, 180);
}

void ProfileManagerTest::testProfileDeletion()
{
    // Test deleting a profile
    QVERIFY(m_profileManager->deleteProfile("test_user"));
    
    // Test that profile was deleted
    UserProfile retrieved = m_profileManager->getProfile("test_user");
    QVERIFY(retrieved.id.isEmpty());
    
    // Test that parent profile cannot be deleted
    QVERIFY(!m_profileManager->deleteProfile("parent"));
}

void ProfileManagerTest::testCurrentProfileSwitching()
{
    // Test setting current profile
    QString originalProfile = m_profileManager->getCurrentProfileId();
    
    // Create a test profile
    UserProfile profile;
    profile.id = "switch_test";
    profile.name = "Switch Test";
    profile.type = ProfileType::Teen;
    profile.isActive = true;
    m_profileManager->createProfile(profile);
    
    // Switch to the new profile
    m_profileManager->setCurrentProfile("switch_test");
    QCOMPARE(m_profileManager->getCurrentProfileId(), QString("switch_test"));
    
    // Switch back
    m_profileManager->setCurrentProfile(originalProfile);
    QCOMPARE(m_profileManager->getCurrentProfileId(), originalProfile);
    
    // Clean up
    m_profileManager->deleteProfile("switch_test");
}

void ProfileManagerTest::testProfileSwitching()
{
    // Test switching to next/previous profile
    QString originalProfile = m_profileManager->getCurrentProfileId();
    
    // Create additional profiles for testing
    UserProfile profile1;
    profile1.id = "profile1";
    profile1.name = "Profile 1";
    profile1.type = ProfileType::Child;
    profile1.isActive = true;
    m_profileManager->createProfile(profile1);
    
    UserProfile profile2;
    profile2.id = "profile2";
    profile2.name = "Profile 2";
    profile2.type = ProfileType::Teen;
    profile2.isActive = true;
    m_profileManager->createProfile(profile2);
    
    // Test switching to next profile
    m_profileManager->switchToNextProfile();
    QString currentProfile = m_profileManager->getCurrentProfileId();
    QVERIFY(currentProfile != originalProfile);
    
    // Test switching to previous profile
    m_profileManager->switchToPreviousProfile();
    QCOMPARE(m_profileManager->getCurrentProfileId(), originalProfile);
    
    // Clean up
    m_profileManager->deleteProfile("profile1");
    m_profileManager->deleteProfile("profile2");
}

QTEST_MAIN(ProfileManagerTest)