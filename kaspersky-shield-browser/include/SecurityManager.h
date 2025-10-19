#ifndef SECURITYMANAGER_H
#define SECURITYMANAGER_H

#include <QObject>
#include <QUrl>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "CommonTypes.h"

class SecurityManager : public QObject
{
    Q_OBJECT

public:
    explicit SecurityManager(QObject *parent = nullptr);
    ~SecurityManager();

    UrlCheckResult checkUrl(const QUrl &url);
    void checkUrlAsync(const QUrl &url);

signals:
    void urlCheckCompleted(const QUrl &url, const UrlCheckResult &result);

private slots:
    void onUrlCheckFinished();

private:
    QNetworkAccessManager *m_networkManager;
    QNetworkReply *m_currentReply;
    QUrl m_currentUrl;
    
    UrlCheckResult parseKsnResponse(const QByteArray &response);
    UrlSafety classifyUrl(const QString &url);
};

#endif // SECURITYMANAGER_H