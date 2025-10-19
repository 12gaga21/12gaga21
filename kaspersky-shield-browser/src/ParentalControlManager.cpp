#include "ParentalControlManager.h"
#include <QTimer>
#include <QDateTime>
#include <QTime>
#include <QDebug>

ParentalControlManager::ParentalControlManager(QObject *parent)
    : QObject(parent)
    , m_usageTimer(new QTimer(this))
{
    // Set up usage timer to update every minute
    m_usageTimer->setInterval(60000); // 1 minute
    connect(m_usageTimer, &QTimer::timeout, this, &ParentalControlManager::updateUsageTime);
    m_usageTimer->start();
    
    // Create default parent profile
    UserProfile parentProfile;
    parentProfile.id = "parent";
    parentProfile.name = "Parent";
    parentProfile.type = ProfileType::Parent;
    parentProfile.timeLimitMinutes = 0; // No limit
    parentProfile.isActive = true;
    createProfile(parentProfile);
}

ParentalControlManager::~ParentalControlManager()
{
    // Cleanup is handled by Qt's parent-child system
}

bool ParentalControlManager::createProfile(const UserProfile &profile)
{
    if (m_profiles.contains(profile.id)) {
        qWarning() << "Profile with ID" << profile.id << "already exists";
        return false;
    }
    
    m_profiles[profile.id] = profile;
    emit profileCreated(profile.id);
    qDebug() << "Created profile:" << profile.name << "(" << profile.id << ")";
    return true;
}

bool ParentalControlManager::updateProfile(const UserProfile &profile)
{
    if (!m_profiles.contains(profile.id)) {
        qWarning() << "Profile with ID" << profile.id << "does not exist";
        return false;
    }
    
    m_profiles[profile.id] = profile;
    emit profileUpdated(profile.id);
    qDebug() << "Updated profile:" << profile.name << "(" << profile.id << ")";
    return true;
}

bool ParentalControlManager::deleteProfile(const QString &profileId)
{
    if (!m_profiles.contains(profileId)) {
        qWarning() << "Profile with ID" << profileId << "does not exist";
        return false;
    }
    
    if (profileId == "parent") {
        qWarning() << "Cannot delete parent profile";
        return false;
    }
    
    m_profiles.remove(profileId);
    emit profileDeleted(profileId);
    qDebug() << "Deleted profile:" << profileId;
    return true;
}

UserProfile ParentalControlManager::getProfile(const QString &profileId) const
{
    return m_profiles.value(profileId, UserProfile());
}

QList<UserProfile> ParentalControlManager::getAllProfiles() const
{
    return m_profiles.values();
}

bool ParentalControlManager::isUrlAllowed(const QUrl &url, const QString &profileId)
{
    if (!m_profiles.contains(profileId)) {
        qWarning() << "Profile with ID" << profileId << "does not exist";
        return true; // Allow by default if profile doesn't exist
    }
    
    const UserProfile &profile = m_profiles[profileId];
    
    // Check if profile is active
    if (!profile.isActive) {
        return false;
    }
    
    // Check time restrictions
    if (!isTimeAllowed(profileId)) {
        return false;
    }
    
    // Check content categories
    QStringList categories = categorizeUrl(url);
    for (const QString &category : categories) {
        if (isCategoryBlocked(profileId, category)) {
            logBlockedAccess(profileId, url, category);
            return false;
        }
    }
    
    return true;
}

bool ParentalControlManager::isTimeAllowed(const QString &profileId)
{
    if (!m_profiles.contains(profileId)) {
        return true;
    }
    
    const UserProfile &profile = m_profiles[profileId];
    
    // Check daily time limit
    if (profile.timeLimitMinutes > 0) {
        int remainingTime = getRemainingTime(profileId);
        if (remainingTime <= 0) {
            emit timeLimitReached(profileId);
            return false;
        }
    }
    
    // Check bedtime restrictions
    QTime currentTime = QTime::currentTime();
    if (profile.bedTimeStart.isValid() && profile.bedTimeEnd.isValid()) {
        if (profile.bedTimeStart < profile.bedTimeEnd) {
            // Normal case: bedtime is within the same day
            if (currentTime >= profile.bedTimeStart && currentTime <= profile.bedTimeEnd) {
                emit bedTimeReached(profileId);
                return false;
            }
        } else {
            // Bedtime spans midnight
            if (currentTime >= profile.bedTimeStart || currentTime <= profile.bedTimeEnd) {
                emit bedTimeReached(profileId);
                return false;
            }
        }
    }
    
    return true;
}

QStringList ParentalControlManager::getBlockedCategories(const QString &profileId) const
{
    // This is a simplified implementation
    // In a real system, this would return categories blocked for the profile
    QStringList blockedCategories;
    
    if (m_profiles.contains(profileId)) {
        const UserProfile &profile = m_profiles[profileId];
        if (profile.type == ProfileType::Child) {
            blockedCategories << "Adult" << "Violence" << "Gambling" << "Social Media";
        } else if (profile.type == ProfileType::Teen) {
            blockedCategories << "Adult" << "Gambling";
        }
    }
    
    return blockedCategories;
}

void ParentalControlManager::logActivity(const ActivityRecord &record)
{
    // In a real implementation, this would save to database
    qDebug() << "Activity logged:" << record.profileId << record.url.toString() 
             << "Blocked:" << record.wasBlocked;
}

QList<ActivityRecord> ParentalControlManager::getActivityHistory(const QString &profileId) const
{
    // This is a placeholder implementation
    // In a real system, this would query the database
    return QList<ActivityRecord>();
}

void ParentalControlManager::startSession(const QString &profileId)
{
    if (m_profiles.contains(profileId)) {
        m_sessionStarts[profileId] = QDateTime::currentDateTime();
        qDebug() << "Session started for profile:" << profileId;
    }
}

void ParentalControlManager::endSession(const QString &profileId)
{
    if (m_sessionStarts.contains(profileId)) {
        QDateTime sessionStart = m_sessionStarts[profileId];
        int sessionMinutes = sessionStart.secsTo(QDateTime::currentDateTime()) / 60;
        
        // Update profile usage
        if (m_profiles.contains(profileId)) {
            UserProfile profile = m_profiles[profileId];
            profile.currentUsageMinutes += sessionMinutes;
            m_profiles[profileId] = profile;
        }
        
        m_sessionStarts.remove(profileId);
        qDebug() << "Session ended for profile:" << profileId << "Duration:" << sessionMinutes << "minutes";
    }
}

int ParentalControlManager::getRemainingTime(const QString &profileId) const
{
    if (!m_profiles.contains(profileId)) {
        return 0;
    }
    
    const UserProfile &profile = m_profiles[profileId];
    if (profile.timeLimitMinutes <= 0) {
        return -1; // No limit
    }
    
    int used = profile.currentUsageMinutes;
    if (m_sessionStarts.contains(profileId)) {
        QDateTime sessionStart = m_sessionStarts[profileId];
        int sessionMinutes = sessionStart.secsTo(QDateTime::currentDateTime()) / 60;
        used += sessionMinutes;
    }
    
    return qMax(0, profile.timeLimitMinutes - used);
}

void ParentalControlManager::updateUsageTime()
{
    // This method is called every minute to update usage time
    // In a real implementation, this would also check for time limits
    for (auto it = m_sessionStarts.begin(); it != m_sessionStarts.end(); ++it) {
        const QString &profileId = it.key();
        if (m_profiles.contains(profileId)) {
            // Check if time limit is reached
            int remainingTime = getRemainingTime(profileId);
            if (remainingTime == 0) {
                emit timeLimitReached(profileId);
            }
        }
    }
}

QStringList ParentalControlManager::categorizeUrl(const QUrl &url) const
{
    // Simple URL categorization
    // In a real implementation, this would use a more sophisticated system
    QStringList categories;
    QString host = url.host().toLower();
    
    if (host.contains("youtube.com") || host.contains("vimeo.com")) {
        categories << "Video";
    }
    if (host.contains("facebook.com") || host.contains("twitter.com") || host.contains("instagram.com")) {
        categories << "Social Media";
    }
    if (host.contains("porn") || host.contains("adult")) {
        categories << "Adult";
    }
    if (host.contains("casino") || host.contains("poker") || host.contains("bet")) {
        categories << "Gambling";
    }
    if (host.contains("news") || host.contains("cnn.com") || host.contains("bbc.com")) {
        categories << "News";
    }
    
    if (categories.isEmpty()) {
        categories << "General";
    }
    
    return categories;
}

bool ParentalControlManager::isCategoryBlocked(const QString &profileId, const QString &category) const
{
    if (!m_profiles.contains(profileId)) {
        return false;
    }
    
    const UserProfile &profile = m_profiles[profileId];
    
    // Parent profiles have no restrictions
    if (profile.type == ProfileType::Parent) {
        return false;
    }
    
    // Child profiles have more restrictions
    if (profile.type == ProfileType::Child) {
        return category == "Adult" || category == "Violence" || category == "Gambling";
    }
    
    // Teen profiles have moderate restrictions
    if (profile.type == ProfileType::Teen) {
        return category == "Adult" || category == "Gambling";
    }
    
    return false;
}

void ParentalControlManager::logBlockedAccess(const QString &profileId, const QUrl &url, const QString &category)
{
    ActivityRecord record;
    record.profileId = profileId;
    record.url = url;
    record.timestamp = QDateTime::currentDateTime();
    record.category = category;
    record.wasBlocked = true;
    record.blockReason = "Category blocked: " + category;
    
    logActivity(record);
    emit urlBlocked(profileId, url, record.blockReason);
}