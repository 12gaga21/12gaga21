#ifndef PROFILEMANAGER_H
#define PROFILEMANAGER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QTimer>
#include <QHash>
#include <optional>
#include "CommonTypes.h"

class DatabaseManager;

/**
 * @brief ProfileManager - Менеджер профилей пользователей
 * 
 * Управляет профилями пользователей с интеграцией DatabaseManager
 * для персистентного хранения данных.
 */
class ProfileManager : public QObject
{
    Q_OBJECT

public:
    explicit ProfileManager(QObject *parent = nullptr);
    ~ProfileManager();

    /**
     * @brief Инициализация с DatabaseManager
     * @param dbManager Менеджер базы данных
     * @return true если инициализация успешна
     */
    bool initialize(DatabaseManager *dbManager);

    // Profile management
    bool createProfile(const UserProfile &profile);
    bool updateProfile(const UserProfile &profile);
    bool deleteProfile(const QString &profileId);
    UserProfile getProfile(const QString &profileId) const;
    std::optional<UserProfile> getProfileOpt(const QString &profileId) const;
    QList<UserProfile> getAllProfiles() const;
    
    // Current profile
    QString getCurrentProfileId() const;
    QString getActiveProfileId() const;
    void setCurrentProfile(const QString &profileId);
    void setActiveProfile(const QString &profileId);
    UserProfile getCurrentProfile() const;
    
    // Profile switching
    void switchToProfile(const QString &profileId);
    void switchToNextProfile();
    void switchToPreviousProfile();
    
    // Database integration
    bool loadProfilesFromDatabase();
    bool saveProfilesToDatabase();
    bool saveProfile(const UserProfile &profile);

signals:
    void profileCreated(const QString &profileId);
    void profileUpdated(const QString &profileId);
    void profileDeleted(const QString &profileId);
    void currentProfileChanged(const QString &profileId);
    void profileSwitched(const QString &fromProfileId, const QString &toProfileId);
    void profileLoadedFromDatabase(const QString &profileId);
    void profileSavedToDatabase(const QString &profileId);

private:
    QList<UserProfile> m_profiles;
    QHash<QString, UserProfile> m_profilesHash; // For faster lookup
    QString m_currentProfileId;
    QTimer *m_usageTimer;
    DatabaseManager *m_dbManager;
    
    void updateUsageTime();
    void resetDailyUsage();
    void rebuildProfileHash();
};

#endif // PROFILEMANAGER_H