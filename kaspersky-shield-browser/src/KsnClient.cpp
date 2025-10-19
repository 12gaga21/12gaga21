#include "KsnClient.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

KsnClient::KsnClient(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_currentReply(nullptr)
{
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &KsnClient::onUrlCheckFinished);
}

KsnClient::~KsnClient()
{
    if (m_currentReply) {
        m_currentReply->deleteLater();
    }
}

UrlCheckResult KsnClient::checkUrl(const QUrl &url)
{
    // This is a placeholder implementation
    // In a real implementation, this would make a synchronous HTTP request to KSN API
    
    UrlCheckResult result;
    result.safety = UrlSafety::Safe;
    result.threatType = "None";
    result.category = "General";
    result.reputationScore = 100;
    result.shouldBlock = false;
    
    // Basic URL validation
    if (url.scheme() != "http" && url.scheme() != "https") {
        result.safety = UrlSafety::Dangerous;
        result.threatType = "Invalid Protocol";
        result.shouldBlock = true;
    }
    
    return result;
}

void KsnClient::checkUrlAsync(const QUrl &url)
{
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
    }
    
    m_currentUrl = url;
    
    // Create request
    QNetworkRequest request;
    request.setUrl(QUrl("https://ksn.kaspersky.com/api/urlcheck")); // Placeholder URL
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    // Create payload
    QJsonObject payload = createRequestPayload(url);
    QJsonDocument doc(payload);
    QByteArray data = doc.toJson();
    
    // Make request
    m_currentReply = m_networkManager->post(request, data);
    
    qDebug() << "Checking URL asynchronously:" << url.toString();
}

void KsnClient::onUrlCheckFinished()
{
    if (!m_currentReply) return;
    
    UrlCheckResult result;
    
    if (m_currentReply->error() == QNetworkReply::NoError) {
        QByteArray data = m_currentReply->readAll();
        result = parseKsnResponse(data);
    } else {
        qWarning() << "KSN API request failed:" << m_currentReply->errorString();
        // Fallback to basic check
        result = checkUrl(m_currentUrl);
    }
    
    emit urlCheckCompleted(m_currentUrl, result);
    
    m_currentReply->deleteLater();
    m_currentReply = nullptr;
}

UrlCheckResult KsnClient::parseKsnResponse(const QByteArray &response)
{
    UrlCheckResult result;
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(response, &error);
    
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse KSN response:" << error.errorString();
        result.safety = UrlSafety::Safe; // Default to safe on parse error
        result.threatType = "Parse Error";
        result.category = "Unknown";
        result.reputationScore = 50;
        result.shouldBlock = false;
        return result;
    }
    
    QJsonObject obj = doc.object();
    
    // Parse KSN API response
    // This is a placeholder - actual KSN API response format would be different
    result.safety = obj.value("isSafe").toBool(true) ? UrlSafety::Safe : UrlSafety::Dangerous;
    result.threatType = obj.value("threatType").toString("None");
    result.category = obj.value("category").toString("General");
    result.reputationScore = obj.value("reputationScore").toInt(100);
    result.shouldBlock = obj.value("shouldBlock").toBool(false);
    
    return result;
}

QJsonObject KsnClient::createRequestPayload(const QUrl &url)
{
    QJsonObject payload;
    payload["url"] = url.toString();
    payload["version"] = "1.0";
    payload["client"] = "KasperskyShieldBrowser";
    payload["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    return payload;
}