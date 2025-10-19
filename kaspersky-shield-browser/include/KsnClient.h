#ifndef KSNCLIENT_H
#define KSNCLIENT_H

#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include "CommonTypes.h"

class KsnClient : public QObject
{
    Q_OBJECT

public:
    explicit KsnClient(QObject *parent = nullptr);
    ~KsnClient();

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
    QJsonObject createRequestPayload(const QUrl &url);
};

#endif // KSNCLIENT_H