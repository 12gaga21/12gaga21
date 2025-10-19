#ifndef SECURITYMANAGER_H
#define SECURITYMANAGER_H

#include <QObject>
#include <QUrl>
#include <QString>
#include <QCache>
#include "CommonTypes.h"
#include "KsnClient.h"

/**
 * @brief SecurityManager - Менеджер безопасности браузера
 * 
 * Управляет безопасностью браузера через интеграцию с KsnClient.
 * Обеспечивает проверку URL, блокировку угроз и логирование событий.
 */
class SecurityManager : public QObject
{
    Q_OBJECT

public:
    explicit SecurityManager(QObject *parent = nullptr);
    ~SecurityManager();

    /**
     * @brief Инициализация менеджера безопасности
     * @param ksnClient Клиент KSN API
     * @return true если инициализация успешна
     */
    bool initialize(KsnClient *ksnClient);

    /**
     * @brief Синхронная проверка URL
     * @param url URL для проверки
     * @return Результат проверки
     */
    UrlCheckResult checkUrl(const QUrl &url);

    /**
     * @brief Асинхронная проверка URL
     * @param url URL для проверки
     */
    void checkUrlAsync(const QUrl &url);

    /**
     * @brief Проверка должен ли URL быть заблокирован
     * @param url URL для проверки
     * @return true если URL должен быть заблокирован
     */
    bool shouldBlockUrl(const QUrl &url);

    /**
     * @brief Логирование события безопасности
     * @param url URL
     * @param result Результат проверки
     */
    void logSecurityEvent(const QUrl &url, const UrlCheckResult &result);

    /**
     * @brief Получение статистики безопасности
     * @return Статистика проверок
     */
    struct SecurityStats {
        int totalChecks;
        int blockedUrls;
        int safeUrls;
        int suspiciousUrls;
        int cacheHits;
    };
    SecurityStats getStatistics() const;

signals:
    void urlCheckCompleted(const QUrl &url, const UrlCheckResult &result);
    void urlBlocked(const QUrl &url, const QString &reason);
    void threatDetected(const QUrl &url, const QString &threatType);

private slots:
    void onKsnCheckCompleted(const QUrl &url, const KsnClient::UrlCheckResponse &response);

private:
    KsnClient *m_ksnClient;
    QCache<QString, UrlCheckResult> m_cache;
    
    // Статистика
    int m_totalChecks;
    int m_blockedUrls;
    int m_safeUrls;
    int m_suspiciousUrls;
    int m_cacheHits;
    
    /**
     * @brief Конвертация результата KSN в UrlCheckResult
     */
    UrlCheckResult convertKsnResult(const KsnClient::UrlCheckResponse &response);
    
    /**
     * @brief Проверка в локальном кэше
     */
    UrlCheckResult* checkCache(const QString &url);
    
    /**
     * @brief Добавление в кэш
     */
    void addToCache(const QString &url, const UrlCheckResult &result);
    
    /**
     * @brief Базовая классификация URL (без KSN)
     */
    UrlSafety classifyUrlBasic(const QString &url);
    
    /**
     * @brief Конвертация категории в строку
     */
    QString categoryToString(KsnClient::SiteCategory category);
};

#endif // SECURITYMANAGER_H