#ifndef COMMONTYPES_H
#define COMMONTYPES_H

#include <QString>
#include <QUrl>
#include <QTime>
#include <QDateTime>

enum class ProfileType {
    Parent,
    Child,
    Teen
};

struct UserProfile {
    QString id;
    QString name;
    ProfileType type;
    int timeLimitMinutes;
    QTime bedTimeStart;
    QTime bedTimeEnd;
    QDateTime lastResetDate;
    int currentUsageMinutes;
    bool isActive;
    QString avatarPath;
};

enum class UrlSafety {
    Safe,
    Suspicious,
    Dangerous
};

struct UrlCheckResult {
    UrlSafety safety;
    QString threatType;
    QString category;
    int reputationScore;
    bool shouldBlock;
};

struct SecurityEvent {
    QString type;
    QString severity;
    QString details;
    QDateTime timestamp;
};

struct ActivityRecord {
    QString profileId;
    QUrl url;
    QString title;
    QDateTime timestamp;
    QString category;
    bool wasBlocked;
    QString blockReason;
};

#endif // COMMONTYPES_H