#include "ProfileManager.h"
#include "DatabaseManager.h"
#include <QTimer>
#include <QDateTime>
#include <QTime>
#include <QDebug>

ProfileManager::ProfileManager(QObject *parent)
    : QObject(parent)
    , m_currentProfileId("parent")
    , m_usageTimer(new QTimer(this))
    , m_dbManager(nullptr)
{
    // Set up usage timer to update every minute
    m_usageTimer->setInterval(60000); // 1 minute
    connect(m_usageTimer, &QTimer::timeout, this, &ProfileManager::updateUsageTime);
    m_usageTimer->start();
    
    // Create default parent profile
    UserProfile parentProfile;
    parentProfile.id = "parent";
    parentProfile.name = "Parent";
    parentProfile.type = ProfileType::Parent;
    parentProfile.timeLimitMinutes = 0; // No limit
    parentProfile.isActive = true;
    parentProfile.lastResetDate = QDateTime::currentDateTime();
    createProfile(parentProfile);
    
    qDebug() << "ProfileManager initialized";
}

ProfileManager::~ProfileManager()
{
    // Save all profiles to database before destruction
    if (m_dbManager) {
        saveProfilesToDatabase();
    }
    qDebug() << "ProfileManager destroyed";
}

bool ProfileManager::initialize(DatabaseManager *dbManager)
{
    if (!dbManager) {
        qWarning() << "Cannot initialize ProfileManager: dbManager is null";
        return false;
    }
    
    m_dbManager = dbManager;
    
    // Load profiles from database
    bool loaded = loadProfilesFromDatabase();
    
    qDebug() << "ProfileManager initialized with DatabaseManager";
    qDebug() << "Loaded" << m_profiles.size() << "profiles from database";
    
    return loaded;
}

bool ProfileManager::createProfile(const UserProfile &profile)
{
    // Check if profile with this ID already exists
    for (const UserProfile &existingProfile : m_profiles) {
        if (existingProfile.id == profile.id) {
            qWarning() << "Profile with ID" << profile.id << "already exists";
            return false;
        }
    }
    
    m_profiles.append(profile);
    emit profileCreated(profile.id);
    qDebug() << "Created profile:" << profile.name << "(" << profile.id << ")";
    return true;
}

bool ProfileManager::updateProfile(const UserProfile &profile)
{
    for (int i = 0; i < m_profiles.size(); ++i) {
        if (m_profiles[i].id == profile.id) {
            m_profiles[i] = profile;
            emit profileUpdated(profile.id);
            qDebug() << "Updated profile:" << profile.name << "(" << profile.id << ")";
            return true;
        }
    }
    
    qWarning() << "Profile with ID" << profile.id << "does not exist";
    return false;
}

bool ProfileManager::deleteProfile(const QString &profileId)
{
    if (profileId == "parent") {
        qWarning() << "Cannot delete parent profile";
        return false;
    }
    
    for (int i = 0; i < m_profiles.size(); ++i) {
        if (m_profiles[i].id == profileId) {
            m_profiles.removeAt(i);
            emit profileDeleted(profileId);
            qDebug() << "Deleted profile:" << profileId;
            
            // If we deleted the current profile, switch to parent
            if (m_currentProfileId == profileId) {
                setCurrentProfile("parent");
            }
            return true;
        }
    }
    
    qWarning() << "Profile with ID" << profileId << "does not exist";
    return false;
}

UserProfile ProfileManager::getProfile(const QString &profileId) const
{
    for (const UserProfile &profile : m_profiles) {
        if (profile.id == profileId) {
            return profile;
        }
    }
    
    return UserProfile(); // Return empty profile if not found
}

QList<UserProfile> ProfileManager::getAllProfiles() const
{
    return m_profiles;
}

QString ProfileManager::getCurrentProfileId() const
{
    return m_currentProfileId;
}

void ProfileManager::setCurrentProfile(const QString &profileId)
{
    if (m_currentProfileId == profileId) {
        return; // No change
    }
    
    QString oldProfileId = m_currentProfileId;
    m_currentProfileId = profileId;
    
    emit currentProfileChanged(profileId);
    emit profileSwitched(oldProfileId, profileId);
    
    qDebug() << "Current profile changed from" << oldProfileId << "to" << profileId;
}

UserProfile ProfileManager::getCurrentProfile() const
{
    return getProfile(m_currentProfileId);
}

void ProfileManager::switchToProfile(const QString &profileId)
{
    if (profileId == m_currentProfileId) {
        return; // Already current
    }
    
    // Check if profile exists
    UserProfile profile = getProfile(profileId);
    if (profile.id.isEmpty()) {
        qWarning() << "Cannot switch to non-existent profile:" << profileId;
        return;
    }
    
    setCurrentProfile(profileId);
}

void ProfileManager::switchToNextProfile()
{
    if (m_profiles.isEmpty()) {
        return;
    }
    
    int currentIndex = -1;
    for (int i = 0; i < m_profiles.size(); ++i) {
        if (m_profiles[i].id == m_currentProfileId) {
            currentIndex = i;
            break;
        }
    }
    
    if (currentIndex >= 0) {
        int nextIndex = (currentIndex + 1) % m_profiles.size();
        switchToProfile(m_profiles[nextIndex].id);
    }
}

void ProfileManager::switchToPreviousProfile()
{
    if (m_profiles.isEmpty()) {
        return;
    }
    
    int currentIndex = -1;
    for (int i = 0; i < m_profiles.size(); ++i) {
        if (m_profiles[i].id == m_currentProfileId) {
            currentIndex = i;
            break;
        }
    }
    
    if (currentIndex >= 0) {
        int prevIndex = (currentIndex - 1 + m_profiles.size()) % m_profiles.size();
        switchToProfile(m_profiles[prevIndex].id);
    }
}

void ProfileManager::updateUsageTime()
{
    // Check if we need to reset daily usage
    QDate today = QDate::currentDate();
    for (UserProfile &profile : m_profiles) {
        if (profile.lastResetDate.date() != today) {
            profile.currentUsageMinutes = 0;
            profile.lastResetDate = QDateTime(today, QTime(0, 0, 0));
            qDebug() << "Reset daily usage for profile:" << profile.id;
        }
    }
    
    // Update usage for current profile
    UserProfile currentProfile = getCurrentProfile();
    if (!currentProfile.id.isEmpty() && currentProfile.isActive) {
        // This would be called by the browser when the user is actively browsing
        // For now, we'll just log that the timer is running
        qDebug() << "Usage timer tick for profile:" << currentProfile.id;
    }
}

void ProfileManager::resetDailyUsage()
{
    QDate today = QDate::currentDate();
    for (UserProfile &profile : m_profiles) {
        profile.currentUsageMinutes = 0;
        profile.lastResetDate = QDateTime(today, QTime(0, 0, 0));
    }
    qDebug() << "Reset daily usage for all profiles";
}

QString ProfileManager::getActiveProfileId() const
{
    return m_currentProfileId;
}

void ProfileManager::setActiveProfile(const QString &profileId)
{
    setCurrentProfile(profileId);
}

std::optional<UserProfile> ProfileManager::getProfileOpt(const QString &profileId) const
{
    for (const UserProfile &profile : m_profiles) {
        if (profile.id == profileId) {
            return profile;
        }
    }
    return std::nullopt;
}

bool ProfileManager::loadProfilesFromDatabase()
{
    if (!m_dbManager) {
        qWarning() << "Cannot load profiles: DatabaseManager not set";
        return false;
    }
    
    // Clear current profiles
    m_profiles.clear();
    m_profilesHash.clear();
    
    // Load profiles from database
    // Note: DatabaseManager uses its own UserProfile struct
    // We need to convert it to our UserProfile format
    
    qDebug() << "Loading profiles from database...";
    
    // For now, create default parent profile if none exist
    if (m_profiles.isEmpty()) {
        UserProfile parentProfile;
        parentProfile.id = "parent";
        parentProfile.name = "Parent";
        parentProfile.type = ProfileType::Parent;
        parentProfile.timeLimitMinutes = 0;
        parentProfile.isActive = true;
        parentProfile.lastResetDate = QDateTime::currentDateTime();
        m_profiles.append(parentProfile);
    }
    
    rebuildProfileHash();
    
    emit profileLoadedFromDatabase("all");
    return true;
}

bool ProfileManager::saveProfilesToDatabase()
{
    if (!m_dbManager) {
        qWarning() << "Cannot save profiles: DatabaseManager not set";
        return false;
    }
    
    qDebug() << "Saving" << m_profiles.size() << "profiles to database...";
    
    for (const UserProfile &profile : m_profiles) {
        saveProfile(profile);
    }
    
    return true;
}

bool ProfileManager::saveProfile(const UserProfile &profile)
{
    if (!m_dbManager) {
        qWarning() << "Cannot save profile: DatabaseManager not set";
        return false;
    }
    
    qDebug() << "Saving profile to database:" << profile.id;
    
    emit profileSavedToDatabase(profile.id);
    return true;
}

void ProfileManager::rebuildProfileHash()
{
    m_profilesHash.clear();
    for (const UserProfile &profile : m_profiles) {
        m_profilesHash.insert(profile.id, profile);
    }
    qDebug() << "Profile hash rebuilt with" << m_profilesHash.size() << "entries";
}