#include "SecurityManager.h"
#include <QDebug>
#include <QDateTime>

SecurityManager::SecurityManager(QObject *parent)
    : QObject(parent)
    , m_ksnClient(nullptr)
    , m_cache(1000) // Cache size
    , m_totalChecks(0)
    , m_blockedUrls(0)
    , m_safeUrls(0)
    , m_suspiciousUrls(0)
    , m_cacheHits(0)
{
    qDebug() << "SecurityManager initialized";
}

SecurityManager::~SecurityManager()
{
    qDebug() << "SecurityManager destroyed";
}

bool SecurityManager::initialize(KsnClient *ksnClient)
{
    if (!ksnClient) {
        qWarning() << "Cannot initialize SecurityManager: ksnClient is null";
        return false;
    }
    
    m_ksnClient = ksnClient;
    
    // Connect to KSN client signals
    connect(m_ksnClient, &KsnClient::urlCheckCompleted,
            this, &SecurityManager::onKsnCheckCompleted);
    
    qDebug() << "SecurityManager initialized with KsnClient";
    return true;
}

UrlCheckResult SecurityManager::checkUrl(const QUrl &url)
{
    m_totalChecks++;
    
    QString urlString = url.toString();
    
    // Check cache first
    UrlCheckResult *cached = checkCache(urlString);
    if (cached) {
        m_cacheHits++;
        qDebug() << "Cache hit for URL:" << urlString;
        return *cached;
    }
    
    // If KSN client is available, use it
    if (m_ksnClient) {
        auto ksnResponse = m_ksnClient->checkUrlSync(url);
        UrlCheckResult result = convertKsnResult(ksnResponse);
        
        // Add to cache
        addToCache(urlString, result);
        
        // Update statistics
        if (result.safety == UrlSafety::Safe) {
            m_safeUrls++;
        } else if (result.safety == UrlSafety::Dangerous || 
                   result.safety == UrlSafety::Malicious) {
            m_blockedUrls++;
        } else if (result.safety == UrlSafety::Suspicious) {
            m_suspiciousUrls++;
        }
        
        // Log event
        logSecurityEvent(url, result);
        
        return result;
    }
    
    // Fallback to basic classification
    UrlCheckResult result;
    result.safety = classifyUrlBasic(urlString);
    result.threatType = "Unknown";
    result.category = "General";
    result.reputationScore = 50;
    result.shouldBlock = (result.safety == UrlSafety::Dangerous);
    result.isValid = true;
    
    addToCache(urlString, result);
    return result;
}

void SecurityManager::checkUrlAsync(const QUrl &url)
{
    m_totalChecks++;
    
    QString urlString = url.toString();
    
    // Check cache first
    UrlCheckResult *cached = checkCache(urlString);
    if (cached) {
        m_cacheHits++;
        qDebug() << "Cache hit for URL (async):" << urlString;
        emit urlCheckCompleted(url, *cached);
        return;
    }
    
    // Use KSN client for async check
    if (m_ksnClient) {
        m_ksnClient->checkUrl(url);
        // Result will be received via onKsnCheckCompleted signal
    } else {
        // Fallback to sync check
        UrlCheckResult result = checkUrl(url);
        emit urlCheckCompleted(url, result);
    }
}

bool SecurityManager::shouldBlockUrl(const QUrl &url)
{
    UrlCheckResult result = checkUrl(url);
    return result.shouldBlock;
}

void SecurityManager::logSecurityEvent(const QUrl &url, const UrlCheckResult &result)
{
    qDebug() << "Security Event:"
             << "URL:" << url.toString()
             << "Safety:" << static_cast<int>(result.safety)
             << "Blocked:" << result.shouldBlock;
    
    // Emit signals based on result
    if (result.shouldBlock) {
        emit urlBlocked(url, result.threatType);
    }
    
    if (result.safety == UrlSafety::Dangerous || result.safety == UrlSafety::Malicious) {
        emit threatDetected(url, result.threatType);
    }
}

SecurityManager::SecurityStats SecurityManager::getStatistics() const
{
    SecurityStats stats;
    stats.totalChecks = m_totalChecks;
    stats.blockedUrls = m_blockedUrls;
    stats.safeUrls = m_safeUrls;
    stats.suspiciousUrls = m_suspiciousUrls;
    stats.cacheHits = m_cacheHits;
    return stats;
}

void SecurityManager::onKsnCheckCompleted(const QUrl &url, const KsnClient::UrlCheckResponse &response)
{
    // Convert KSN response to UrlCheckResult
    UrlCheckResult result = convertKsnResult(response);
    
    // Add to cache
    addToCache(url.toString(), result);
    
    // Update statistics
    if (result.safety == UrlSafety::Safe) {
        m_safeUrls++;
    } else if (result.safety == UrlSafety::Dangerous || 
               result.safety == UrlSafety::Malicious) {
        m_blockedUrls++;
    } else if (result.safety == UrlSafety::Suspicious) {
        m_suspiciousUrls++;
    }
    
    // Log event
    logSecurityEvent(url, result);
    
    // Emit signal
    emit urlCheckCompleted(url, result);
}

UrlCheckResult SecurityManager::convertKsnResult(const KsnClient::UrlCheckResponse &response)
{
    UrlCheckResult result;
    
    // Convert KsnClient::UrlCheckResult to UrlSafety
    switch (response.result) {
        case KsnClient::UrlCheckResult::Safe:
            result.safety = UrlSafety::Safe;
            break;
        case KsnClient::UrlCheckResult::Malicious:
            result.safety = UrlSafety::Malicious;
            break;
        case KsnClient::UrlCheckResult::Phishing:
            result.safety = UrlSafety::Phishing;
            break;
        case KsnClient::UrlCheckResult::Suspicious:
            result.safety = UrlSafety::Suspicious;
            break;
        case KsnClient::UrlCheckResult::Blocked:
            result.safety = UrlSafety::Blocked;
            break;
        case KsnClient::UrlCheckResult::Unknown:
            result.safety = UrlSafety::Unknown;
            break;
        case KsnClient::UrlCheckResult::Error:
        case KsnClient::UrlCheckResult::Timeout:
            result.safety = UrlSafety::Unknown;
            break;
    }
    
    // Convert category
    result.category = categoryToString(response.category);
    result.threatType = response.reason;
    result.reputationScore = response.confidence;
    result.shouldBlock = (result.safety == UrlSafety::Dangerous || 
                         result.safety == UrlSafety::Malicious ||
                         result.safety == UrlSafety::Phishing ||
                         result.safety == UrlSafety::Blocked);
    result.isValid = true;
    
    return result;
}

QString SecurityManager::categoryToString(KsnClient::SiteCategory category)
{
    switch (category) {
        case KsnClient::SiteCategory::General: return "General";
        case KsnClient::SiteCategory::News: return "News";
        case KsnClient::SiteCategory::Social: return "Social";
        case KsnClient::SiteCategory::Shopping: return "Shopping";
        case KsnClient::SiteCategory::Education: return "Education";
        case KsnClient::SiteCategory::Entertainment: return "Entertainment";
        case KsnClient::SiteCategory::Adult: return "Adult";
        case KsnClient::SiteCategory::Gambling: return "Gambling";
        case KsnClient::SiteCategory::Violence: return "Violence";
        case KsnClient::SiteCategory::Drugs: return "Drugs";
        case KsnClient::SiteCategory::Weapons: return "Weapons";
        case KsnClient::SiteCategory::Terrorism: return "Terrorism";
        case KsnClient::SiteCategory::Unknown: return "Unknown";
    }
    return "Unknown";
}

UrlCheckResult* SecurityManager::checkCache(const QString &url)
{
    return m_cache.object(url);
}

void SecurityManager::addToCache(const QString &url, const UrlCheckResult &result)
{
    m_cache.insert(url, new UrlCheckResult(result));
}

UrlSafety SecurityManager::classifyUrlBasic(const QString &url)
{
    // Basic URL classification logic (fallback when KSN is unavailable)
    QString lowerUrl = url.toLower();
    
    // Check for known malicious patterns
    if (lowerUrl.contains("phishing") || 
        lowerUrl.contains("malware") || 
        lowerUrl.contains("virus") ||
        lowerUrl.contains("trojan")) {
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
        lowerUrl.contains("apple.com") ||
        lowerUrl.contains("github.com") ||
        lowerUrl.contains("wikipedia.org")) {
        return UrlSafety::Safe;
    }
    
    // Default to safe for unknown URLs
    return UrlSafety::Safe;
}
