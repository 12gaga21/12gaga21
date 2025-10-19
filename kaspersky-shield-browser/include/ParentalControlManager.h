#ifndef PARENTALCONTROLMANAGER_H
#define PARENTALCONTROLMANAGER_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QMap>
#include <QTimer>
#include <QDateTime>
#include "CommonTypes.h"

class KsnClient;

/**
 * @brief ParentalControlManager - Менеджер родительского контроля
 * 
 * Управляет родительским контролем с интеграцией KsnClient
 * для определения категорий сайтов и фильтрации контента.
 */
class ParentalControlManager : public QObject
{
    Q_OBJECT

public:
    explicit ParentalControlManager(QObject *parent = nullptr);
    ~ParentalControlManager();

    /**
     * @brief Инициализация с KsnClient
     * @param ksnClient Клиент KSN API
     * @return true если инициализация успешна
     */
    bool initialize(KsnClient *ksnClient);

    // Profile management
    bool createProfile(const UserProfile &profile);
    bool updateProfile(const UserProfile &profile);
    bool deleteProfile(const QString &profileId);
    UserProfile getProfile(const QString &profileId) const;
    QList<UserProfile> getAllProfiles() const;
    
    // Content filtering
    bool isUrlAllowed(const QUrl &url, const QString &profileId);
    bool isTimeAllowed(const QString &profileId);
    QStringList getBlockedCategories(const QString &profileId) const;
    void setBlockedCategories(const QString &profileId, const QStringList &categories);
    
    // Category checking
    QString getUrlCategory(const QUrl &url);
    bool isCategorySafe(const QString &category, ProfileType profileType);
    
    // Activity logging
    void logActivity(const ActivityRecord &record);
    QList<ActivityRecord> getActivityHistory(const QString &profileId) const;
    void clearActivityHistory(const QString &profileId);
    
    // Time management
    void startSession(const QString &profileId);
    void endSession(const QString &profileId);
    int getRemainingTime(const QString &profileId) const;
    void setTimeLimit(const QString &profileId, int minutes);

signals:
    void profileCreated(const QString &profileId);
    void profileUpdated(const QString &profileId);
    void profileDeleted(const QString &profileId);
    void urlBlocked(const QString &profileId, const QUrl &url, const QString &reason);
    void timeLimitReached(const QString &profileId);
    void bedTimeReached(const QString &profileId);
    void activityLogged(const ActivityRecord &record);

private slots:
    void updateUsageTime();

private:
    QMap<QString, UserProfile> m_profiles;
    QMap<QString, QDateTime> m_sessionStarts;
    QMap<QString, QList<ActivityRecord>> m_activityHistory;
    QTimer *m_usageTimer;
    KsnClient *m_ksnClient;
    
    QStringList categorizeUrl(const QUrl &url) const;
    bool isCategoryBlocked(const QString &profileId, const QString &category) const;
    void logBlockedAccess(const QString &profileId, const QUrl &url, const QString &category);
    QStringList getDefaultBlockedCategories(ProfileType profileType) const;
};

#endif // PARENTALCONTROLMANAGER_H