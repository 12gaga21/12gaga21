#include "KsnClient.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QUrlQuery>
#include <QTimer>
#include <QMutexLocker>
#include <QThread>
#include <QLoggingCategory>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QHostInfo>
#include <QSslSocket>
#include <QNetworkProxy>

Q_LOGGING_CATEGORY(ksnClient, "ksn.client")

KsnClient::KsnClient(QObject *parent)
    : QObject(parent)
    , m_networkManager(nullptr)
    , m_queueTimer(new QTimer(this))
    , m_cacheTimer(new QTimer(this))
    , m_activeRequests(0)
    , m_totalRequests(0)
    , m_cachedRequests(0)
    , m_errorRequests(0)
    , m_connected(false)
{
    // Инициализация конфигурации по умолчанию
    m_config.apiKey = "";
    m_config.apiUrl = "https://ksn.kaspersky.com/api/v1";
    m_config.timeoutMs = 10000;
    m_config.maxRetries = 3;
    m_config.cacheSize = 1000;
    m_config.cacheExpiryHours = 24;
    m_config.enableParentalControl = true;
    m_config.enableRealTimeCheck = true;
    m_config.enableCaching = true;
    m_config.enableLogging = true;

    // Настройка таймеров
    m_queueTimer->setSingleShot(false);
    m_queueTimer->setInterval(100); // Обработка очереди каждые 100мс
    connect(m_queueTimer, &QTimer::timeout, this, &KsnClient::processQueue);

    m_cacheTimer->setSingleShot(false);
    m_cacheTimer->setInterval(3600000); // Очистка кэша каждый час
    connect(m_cacheTimer, &QTimer::timeout, this, &KsnClient::cleanupCache);

    // Инициализация сетевого менеджера
    m_networkManager = new QNetworkAccessManager(this);
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &KsnClient::onReplyFinished);

    // Запуск таймеров
    m_queueTimer->start();
    m_cacheTimer->start();

    qCDebug(ksnClient) << "KsnClient initialized";
}

KsnClient::~KsnClient()
{
    qCDebug(ksnClient) << "KsnClient destroyed";
}

bool KsnClient::initialize(const Config &config)
{
    QMutexLocker locker(&m_connectionMutex);
    
    m_config = config;
    
    // Валидация конфигурации
    if (m_config.apiKey.isEmpty()) {
        qCWarning(ksnClient) << "API key is empty";
        emit errorOccurred("API key is required");
        return false;
    }
    
    if (m_config.apiUrl.isEmpty()) {
        qCWarning(ksnClient) << "API URL is empty";
        emit errorOccurred("API URL is required");
        return false;
    }
    
    if (m_config.timeoutMs <= 0) {
        qCWarning(ksnClient) << "Invalid timeout value:" << m_config.timeoutMs;
        emit errorOccurred("Invalid timeout value");
        return false;
    }
    
    // Проверка подключения к API
    if (!validateApiKey()) {
        qCWarning(ksnClient) << "API key validation failed";
        emit errorOccurred("API key validation failed");
        return false;
    }
    
    m_connected = true;
    emit connectionStatusChanged(true);
    
    qCInfo(ksnClient) << "KsnClient initialized successfully";
    qCInfo(ksnClient) << "API URL:" << m_config.apiUrl;
    qCInfo(ksnClient) << "Timeout:" << m_config.timeoutMs << "ms";
    qCInfo(ksnClient) << "Cache size:" << m_config.cacheSize;
    qCInfo(ksnClient) << "Parental control:" << (m_config.enableParentalControl ? "enabled" : "disabled");
    
    return true;
}

bool KsnClient::checkUrl(const QUrl &url)
{
    if (!m_connected) {
        qCWarning(ksnClient) << "Client not connected";
        emit errorOccurred("Client not connected");
        return false;
    }
    
    if (!url.isValid()) {
        qCWarning(ksnClient) << "Invalid URL:" << url.toString();
        emit errorOccurred("Invalid URL");
        return false;
    }
    
    // Проверка кэша
    if (m_config.enableCaching) {
        UrlCheckResponse cachedResult = getCachedResult(url);
        if (cachedResult.result != UrlCheckResult::Unknown) {
            m_cachedRequests.fetchAndAddOrdered(1);
            qCDebug(ksnClient) << "Using cached result for:" << url.toString();
            emit urlChecked(url, cachedResult);
            return true;
        }
    }
    
    // Добавление в очередь
    {
        QMutexLocker locker(&m_queueMutex);
        m_requestQueue.enqueue(url);
    }
    
    qCDebug(ksnClient) << "URL added to queue:" << url.toString();
    return true;
}

KsnClient::UrlCheckResponse KsnClient::checkUrlSync(const QUrl &url)
{
    if (!m_connected) {
        qCWarning(ksnClient) << "Client not connected";
        return {UrlCheckResult::Error, SiteCategory::Unknown, "Client not connected", QDateTime::currentDateTime(), "", 0, false};
    }
    
    if (!url.isValid()) {
        qCWarning(ksnClient) << "Invalid URL:" << url.toString();
        return {UrlCheckResult::Error, SiteCategory::Unknown, "Invalid URL", QDateTime::currentDateTime(), "", 0, false};
    }
    
    // Проверка кэша
    if (m_config.enableCaching) {
        UrlCheckResponse cachedResult = getCachedResult(url);
        if (cachedResult.result != UrlCheckResult::Unknown) {
            m_cachedRequests.fetchAndAddOrdered(1);
            qCDebug(ksnClient) << "Using cached result for:" << url.toString();
            return cachedResult;
        }
    }
    
    // Синхронная проверка
    QNetworkRequest request;
    request.setUrl(QUrl(m_config.apiUrl + "/check"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + m_config.apiKey).toUtf8());
    request.setRawHeader("User-Agent", "KasperskyShieldBrowser/1.0");
    
    QJsonObject jsonData;
    jsonData["url"] = url.toString();
    jsonData["timestamp"] = QDateTime::currentDateTime().toSecsSinceEpoch();
    
    QJsonDocument doc(jsonData);
    QByteArray data = doc.toJson();
    
    QElapsedTimer timer;
    timer.start();
    
    QNetworkReply *reply = m_networkManager->post(request, data);
    
    // Ожидание завершения с таймаутом
    QEventLoop loop;
    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);
    timeoutTimer.setInterval(m_config.timeoutMs);
    
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    connect(&timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);
    
    timeoutTimer.start();
    loop.exec();
    
    UrlCheckResponse response;
    
    if (timeoutTimer.isActive()) {
        // Запрос завершился до таймаута
        timeoutTimer.stop();
        response = parseResponse(reply);
    } else {
        // Таймаут
        qCWarning(ksnClient) << "Request timeout for:" << url.toString();
        response = {UrlCheckResult::Timeout, SiteCategory::Unknown, "Request timeout", QDateTime::currentDateTime(), "", 0, false};
    }
    
    reply->deleteLater();
    
    // Сохранение в кэш
    if (m_config.enableCaching && response.result != UrlCheckResult::Error) {
        cacheResult(url, response);
    }
    
    m_totalRequests.fetchAndAddOrdered(1);
    
    qCDebug(ksnClient) << "Sync check completed for:" << url.toString() 
                       << "in" << timer.elapsed() << "ms";
    
    return response;
}

KsnClient::SiteCategory KsnClient::getSiteCategory(const QUrl &url)
{
    UrlCheckResponse response = checkUrlSync(url);
    return response.category;
}

bool KsnClient::checkParentalControl(const QUrl &url, const QString &profileId)
{
    if (!m_config.enableParentalControl) {
        return true; // Родительский контроль отключен
    }
    
    UrlCheckResponse response = checkUrlSync(url);
    
    // Проверка категории сайта
    switch (response.category) {
        case SiteCategory::Adult:
        case SiteCategory::Gambling:
        case SiteCategory::Violence:
        case SiteCategory::Drugs:
        case SiteCategory::Weapons:
        case SiteCategory::Terrorism:
            return false; // Заблокировано
        default:
            return true; // Разрешено
    }
}

void KsnClient::clearCache()
{
    QMutexLocker locker(&m_cacheMutex);
    m_cache.clear();
    qCInfo(ksnClient) << "Cache cleared";
}

QJsonObject KsnClient::getStatistics() const
{
    QJsonObject stats;
    stats["totalRequests"] = m_totalRequests.load();
    stats["cachedRequests"] = m_cachedRequests.load();
    stats["errorRequests"] = m_errorRequests.load();
    stats["activeRequests"] = m_activeRequests.load();
    stats["cacheSize"] = m_cache.size();
    stats["connected"] = m_connected;
    stats["lastRequestTime"] = m_lastRequestTime.toString(Qt::ISODate);
    
    return stats;
}

bool KsnClient::isConnected() const
{
    QMutexLocker locker(&m_connectionMutex);
    return m_connected;
}

void KsnClient::setApiKey(const QString &apiKey)
{
    m_config.apiKey = apiKey;
    qCDebug(ksnClient) << "API key updated";
}

QString KsnClient::getApiKey() const
{
    return m_config.apiKey;
}

void KsnClient::setTimeout(int timeoutMs)
{
    m_config.timeoutMs = timeoutMs;
    qCDebug(ksnClient) << "Timeout updated to:" << timeoutMs << "ms";
}

int KsnClient::getTimeout() const
{
    return m_config.timeoutMs;
}

void KsnClient::onReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        return;
    }
    
    m_activeRequests.fetchAndSubOrdered(1);
    
    QUrl originalUrl = reply->property("originalUrl").toUrl();
    if (originalUrl.isEmpty()) {
        qCWarning(ksnClient) << "No original URL found in reply";
        reply->deleteLater();
        return;
    }
    
    UrlCheckResponse response = parseResponse(reply);
    
    // Сохранение в кэш
    if (m_config.enableCaching && response.result != UrlCheckResult::Error) {
        cacheResult(originalUrl, response);
    }
    
    m_totalRequests.fetchAndAddOrdered(1);
    
    if (m_config.enableLogging) {
        logRequest(originalUrl, response);
    }
    
    emit urlChecked(originalUrl, response);
    
    reply->deleteLater();
}

void KsnClient::onNetworkError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        return;
    }
    
    m_errorRequests.fetchAndAddOrdered(1);
    
    QString errorString = reply->errorString();
    qCWarning(ksnClient) << "Network error:" << error << errorString;
    
    emit errorOccurred(QString("Network error: %1").arg(errorString));
    
    reply->deleteLater();
}

void KsnClient::onTimeout()
{
    qCWarning(ksnClient) << "Request timeout";
    emit errorOccurred("Request timeout");
}

void KsnClient::processQueue()
{
    if (m_activeRequests.load() >= 10) {
        return; // Ограничение на количество одновременных запросов
    }
    
    QUrl url;
    {
        QMutexLocker locker(&m_queueMutex);
        if (m_requestQueue.isEmpty()) {
            return;
        }
        url = m_requestQueue.dequeue();
    }
    
    sendRequest(url);
}

void KsnClient::cleanupCache()
{
    QMutexLocker locker(&m_cacheMutex);
    
    QDateTime cutoffTime = QDateTime::currentDateTime().addSecs(-m_config.cacheExpiryHours * 3600);
    auto it = m_cache.begin();
    
    while (it != m_cache.end()) {
        if (it.value().timestamp < cutoffTime) {
            it = m_cache.erase(it);
        } else {
            ++it;
        }
    }
    
    qCDebug(ksnClient) << "Cache cleanup completed, size:" << m_cache.size();
}

bool KsnClient::sendRequest(const QUrl &url)
{
    QNetworkRequest request;
    request.setUrl(QUrl(m_config.apiUrl + "/check"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + m_config.apiKey).toUtf8());
    request.setRawHeader("User-Agent", "KasperskyShieldBrowser/1.0");
    
    QJsonObject jsonData;
    jsonData["url"] = url.toString();
    jsonData["timestamp"] = QDateTime::currentDateTime().toSecsSinceEpoch();
    
    QJsonDocument doc(jsonData);
    QByteArray data = doc.toJson();
    
    QNetworkReply *reply = m_networkManager->post(request, data);
    reply->setProperty("originalUrl", url);
    
    // Настройка таймаута
    QTimer *timeoutTimer = new QTimer(this);
    timeoutTimer->setSingleShot(true);
    timeoutTimer->setInterval(m_config.timeoutMs);
    
    connect(timeoutTimer, &QTimer::timeout, [reply, this]() {
        if (reply->isRunning()) {
            reply->abort();
            onTimeout();
        }
    });
    
    connect(reply, &QNetworkReply::finished, timeoutTimer, &QTimer::deleteLater);
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this, &KsnClient::onNetworkError);
    
    timeoutTimer->start();
    m_activeRequests.fetchAndAddOrdered(1);
    m_lastRequestTime = QDateTime::currentDateTime();
    
    qCDebug(ksnClient) << "Request sent for:" << url.toString();
    return true;
}

KsnClient::UrlCheckResponse KsnClient::parseResponse(QNetworkReply *reply)
{
    UrlCheckResponse response;
    response.timestamp = QDateTime::currentDateTime();
    response.isCached = false;
    
    if (reply->error() != QNetworkReply::NoError) {
        response.result = UrlCheckResult::Error;
        response.reason = reply->errorString();
        response.confidence = 0;
        return response;
    }
    
    QByteArray data = reply->readAll();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    
    if (error.error != QJsonParseError::NoError) {
        response.result = UrlCheckResult::Error;
        response.reason = "JSON parse error: " + error.errorString();
        response.confidence = 0;
        return response;
    }
    
    QJsonObject json = doc.object();
    
    // Парсинг результата
    QString status = json["status"].toString();
    if (status == "safe") {
        response.result = UrlCheckResult::Safe;
    } else if (status == "malicious") {
        response.result = UrlCheckResult::Malicious;
    } else if (status == "phishing") {
        response.result = UrlCheckResult::Phishing;
    } else if (status == "suspicious") {
        response.result = UrlCheckResult::Suspicious;
    } else if (status == "blocked") {
        response.result = UrlCheckResult::Blocked;
    } else {
        response.result = UrlCheckResult::Unknown;
    }
    
    // Парсинг категории
    QString category = json["category"].toString();
    if (category == "general") {
        response.category = SiteCategory::General;
    } else if (category == "news") {
        response.category = SiteCategory::News;
    } else if (category == "social") {
        response.category = SiteCategory::Social;
    } else if (category == "shopping") {
        response.category = SiteCategory::Shopping;
    } else if (category == "education") {
        response.category = SiteCategory::Education;
    } else if (category == "entertainment") {
        response.category = SiteCategory::Entertainment;
    } else if (category == "adult") {
        response.category = SiteCategory::Adult;
    } else if (category == "gambling") {
        response.category = SiteCategory::Gambling;
    } else if (category == "violence") {
        response.category = SiteCategory::Violence;
    } else if (category == "drugs") {
        response.category = SiteCategory::Drugs;
    } else if (category == "weapons") {
        response.category = SiteCategory::Weapons;
    } else if (category == "terrorism") {
        response.category = SiteCategory::Terrorism;
    } else {
        response.category = SiteCategory::Unknown;
    }
    
    response.reason = json["reason"].toString();
    response.source = json["source"].toString();
    response.confidence = json["confidence"].toInt(0);
    
    return response;
}

KsnClient::UrlCheckResponse KsnClient::getCachedResult(const QUrl &url) const
{
    QMutexLocker locker(&m_cacheMutex);
    
    QString urlString = url.toString();
    if (m_cache.contains(urlString)) {
        UrlCheckResponse response = m_cache[urlString];
        response.isCached = true;
        return response;
    }
    
    return {UrlCheckResult::Unknown, SiteCategory::Unknown, "", QDateTime(), "", 0, false};
}

void KsnClient::cacheResult(const QUrl &url, const UrlCheckResponse &response)
{
    QMutexLocker locker(&m_cacheMutex);
    
    // Ограничение размера кэша
    if (m_cache.size() >= m_config.cacheSize) {
        // Удаление самой старой записи
        auto oldest = m_cache.begin();
        for (auto it = m_cache.begin(); it != m_cache.end(); ++it) {
            if (it.value().timestamp < oldest.value().timestamp) {
                oldest = it;
            }
        }
        m_cache.erase(oldest);
    }
    
    m_cache[url.toString()] = response;
    qCDebug(ksnClient) << "Result cached for:" << url.toString();
}

bool KsnClient::validateApiKey() const
{
    // Простая валидация API ключа
    if (m_config.apiKey.length() < 32) {
        return false;
    }
    
    // Проверка формата (должен содержать только буквы, цифры и дефисы)
    QRegularExpression regex("^[a-zA-Z0-9-]+$");
    return regex.match(m_config.apiKey).hasMatch();
}

void KsnClient::logRequest(const QUrl &url, const UrlCheckResponse &response)
{
    QString logEntry = QString("[%1] %2 -> %3 (%4) [%5ms]")
        .arg(response.timestamp.toString("yyyy-MM-dd hh:mm:ss"))
        .arg(url.toString())
        .arg(static_cast<int>(response.result))
        .arg(response.reason)
        .arg(response.timestamp.msecsTo(QDateTime::currentDateTime()));
    
    qCDebug(ksnClient) << logEntry;
    
    // Сохранение в файл лога
    QString logPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(logPath);
    
    QFile logFile(logPath + "/ksn_client.log");
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        QTextStream stream(&logFile);
        stream << logEntry << "\n";
    }
}

void KsnClient::updateStatistics()
{
    QJsonObject stats = getStatistics();
    emit statisticsUpdated(stats);
}