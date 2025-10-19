#include "SecurityManager.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QDebug>

SecurityManager::SecurityManager(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_currentReply(nullptr)
{
    // Initialize network manager
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &SecurityManager::onUrlCheckFinished);
}

SecurityManager::~SecurityManager()
{
    if (m_currentReply) {
        m_currentReply->deleteLater();
    }
}

UrlCheckResult SecurityManager::checkUrl(const QUrl &url)
{
    // For now, implement basic URL classification
    // In a real implementation, this would call KSN API
    UrlCheckResult result;
    result.safety = classifyUrl(url.toString());
    result.threatType = "Unknown";
    result.category = "General";
    result.reputationScore = 50;
    result.shouldBlock = (result.safety == UrlSafety::Dangerous);
    
    return result;
}

void SecurityManager::checkUrlAsync(const QUrl &url)
{
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
    }
    
    m_currentUrl = url;
    
    // For demonstration, simulate async check
    // In real implementation, this would make HTTP request to KSN API
    QTimer::singleShot(100, this, [this]() {
        UrlCheckResult result = checkUrl(m_currentUrl);
        emit urlCheckCompleted(m_currentUrl, result);
    });
}

void SecurityManager::onUrlCheckFinished()
{
    if (!m_currentReply) return;
    
    if (m_currentReply->error() == QNetworkReply::NoError) {
        QByteArray data = m_currentReply->readAll();
        UrlCheckResult result = parseKsnResponse(data);
        emit urlCheckCompleted(m_currentUrl, result);
    } else {
        qWarning() << "URL check failed:" << m_currentReply->errorString();
        // Fallback to local classification
        UrlCheckResult result = checkUrl(m_currentUrl);
        emit urlCheckCompleted(m_currentUrl, result);
    }
    
    m_currentReply->deleteLater();
    m_currentReply = nullptr;
}

UrlCheckResult SecurityManager::parseKsnResponse(const QByteArray &response)
{
    // Parse KSN API response
    // This is a placeholder implementation
    UrlCheckResult result;
    result.safety = UrlSafety::Safe;
    result.threatType = "None";
    result.category = "General";
    result.reputationScore = 100;
    result.shouldBlock = false;
    
    return result;
}

UrlSafety SecurityManager::classifyUrl(const QString &url)
{
    // Basic URL classification logic
    // In a real implementation, this would be more sophisticated
    
    QString lowerUrl = url.toLower();
    
    // Check for known malicious patterns
    if (lowerUrl.contains("phishing") || 
        lowerUrl.contains("malware") || 
        lowerUrl.contains("virus")) {
        return UrlSafety::Dangerous;
    }
    
    // Check for suspicious patterns
    if (lowerUrl.contains("suspicious") || 
        lowerUrl.contains("unknown") ||
        lowerUrl.contains("untrusted")) {
        return UrlSafety::Suspicious;
    }
    
    // Check for trusted domains
    if (lowerUrl.contains("kaspersky.com") ||
        lowerUrl.contains("google.com") ||
        lowerUrl.contains("microsoft.com") ||
        lowerUrl.contains("apple.com")) {
        return UrlSafety::Safe;
    }
    
    // Default to safe for now
    return UrlSafety::Safe;
}