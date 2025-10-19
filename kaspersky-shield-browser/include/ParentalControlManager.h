#ifndef PARENTALCONTROLMANAGER_H
#define PARENTALCONTROLMANAGER_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QMap>
#include <QTimer>
#include <QDateTime>
#include "CommonTypes.h"

class ParentalControlManager : public QObject
{
    Q_OBJECT

public:
    explicit ParentalControlManager(QObject *parent = nullptr);
    ~ParentalControlManager();

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
    
    // Activity logging
    void logActivity(const ActivityRecord &record);
    QList<ActivityRecord> getActivityHistory(const QString &profileId) const;
    
    // Time management
    void startSession(const QString &profileId);
    void endSession(const QString &profileId);
    int getRemainingTime(const QString &profileId) const;

signals:
    void profileCreated(const QString &profileId);
    void profileUpdated(const QString &profileId);
    void profileDeleted(const QString &profileId);
    void urlBlocked(const QString &profileId, const QUrl &url, const QString &reason);
    void timeLimitReached(const QString &profileId);
    void bedTimeReached(const QString &profileId);

private slots:
    void updateUsageTime();

private:
    QMap<QString, UserProfile> m_profiles;
    QMap<QString, QDateTime> m_sessionStarts;
    QTimer *m_usageTimer;
    
    QStringList categorizeUrl(const QUrl &url) const;
    bool isCategoryBlocked(const QString &profileId, const QString &category) const;
    void logBlockedAccess(const QString &profileId, const QUrl &url, const QString &category);
};

#endif // PARENTALCONTROLMANAGER_H