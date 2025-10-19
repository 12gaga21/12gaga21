#ifndef PROFILEMANAGER_H
#define PROFILEMANAGER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QTimer>
#include "CommonTypes.h"

class ProfileManager : public QObject
{
    Q_OBJECT

public:
    explicit ProfileManager(QObject *parent = nullptr);
    ~ProfileManager();

    // Profile management
    bool createProfile(const UserProfile &profile);
    bool updateProfile(const UserProfile &profile);
    bool deleteProfile(const QString &profileId);
    UserProfile getProfile(const QString &profileId) const;
    QList<UserProfile> getAllProfiles() const;
    
    // Current profile
    QString getCurrentProfileId() const;
    void setCurrentProfile(const QString &profileId);
    UserProfile getCurrentProfile() const;
    
    // Profile switching
    void switchToProfile(const QString &profileId);
    void switchToNextProfile();
    void switchToPreviousProfile();

signals:
    void profileCreated(const QString &profileId);
    void profileUpdated(const QString &profileId);
    void profileDeleted(const QString &profileId);
    void currentProfileChanged(const QString &profileId);
    void profileSwitched(const QString &fromProfileId, const QString &toProfileId);

private:
    QList<UserProfile> m_profiles;
    QString m_currentProfileId;
    QTimer *m_usageTimer;
    
    void updateUsageTime();
    void resetDailyUsage();
};

#endif // PROFILEMANAGER_H