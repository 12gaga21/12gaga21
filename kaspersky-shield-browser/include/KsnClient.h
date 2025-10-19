#ifndef KSNCLIENT_H
#define KSNCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>
#include <QQueue>
#include <QMutex>
#include <QThread>
#include <QThreadPool>
#include <QRunnable>
#include <QAtomicInt>
#include <QDateTime>
#include <QSettings>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(ksnClient)

/**
 * @brief KsnClient - Клиент для работы с Kaspersky Security Network API
 * 
 * Этот класс обеспечивает интеграцию с KSN API для проверки URL,
 * получения информации о безопасности сайтов и родительского контроля.
 * 
 * Основные возможности:
 * - Проверка URL на безопасность
 * - Получение категории сайта
 * - Родительский контроль
 * - Кэширование результатов
 * - Асинхронная обработка запросов
 * - Обработка ошибок и повторные попытки
 */
class KsnClient : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Результат проверки URL
     */
    enum class UrlCheckResult {
        Safe,           ///< URL безопасен
        Malicious,      ///< URL содержит вредоносный контент
        Phishing,       ///< URL является фишинговым
        Suspicious,     ///< URL подозрительный
        Blocked,        ///< URL заблокирован родительским контролем
        Unknown,        ///< Неизвестный статус
        Error,          ///< Ошибка при проверке
        Timeout         ///< Таймаут запроса
    };

    /**
     * @brief Категория сайта
     */
    enum class SiteCategory {
        General,        ///< Общие сайты
        News,           ///< Новости
        Social,         ///< Социальные сети
        Shopping,       ///< Интернет-магазины
        Education,      ///< Образование
        Entertainment,  ///< Развлечения
        Adult,          ///< Взрослый контент
        Gambling,       ///< Азартные игры
        Violence,       ///< Насилие
        Drugs,          ///< Наркотики
        Weapons,        ///< Оружие
        Terrorism,      ///< Терроризм
        Unknown         ///< Неизвестная категория
    };

    /**
     * @brief Конфигурация KSN клиента
     */
    struct Config {
        QString apiKey;                 ///< API ключ Kaspersky
        QString apiUrl;                 ///< URL API сервера
        int timeoutMs;                  ///< Таймаут запросов в миллисекундах
        int maxRetries;                 ///< Максимальное количество повторов
        int cacheSize;                  ///< Размер кэша результатов
        int cacheExpiryHours;           ///< Время жизни кэша в часах
        bool enableParentalControl;     ///< Включить родительский контроль
        bool enableRealTimeCheck;       ///< Включить проверку в реальном времени
        bool enableCaching;             ///< Включить кэширование
        bool enableLogging;             ///< Включить логирование
    };

    /**
     * @brief Результат проверки URL
     */
    struct UrlCheckResponse {
        UrlCheckResult result;          ///< Результат проверки
        SiteCategory category;          ///< Категория сайта
        QString reason;                 ///< Причина блокировки/предупреждения
        QDateTime timestamp;            ///< Время проверки
        QString source;                 ///< Источник информации
        int confidence;                 ///< Уровень уверенности (0-100)
        bool isCached;                  ///< Результат из кэша
    };

    /**
     * @brief Конструктор
     * @param parent Родительский объект
     */
    explicit KsnClient(QObject *parent = nullptr);

    /**
     * @brief Деструктор
     */
    ~KsnClient();

    /**
     * @brief Инициализация клиента
     * @param config Конфигурация клиента
     * @return true если инициализация успешна
     */
    bool initialize(const Config &config);

    /**
     * @brief Проверка URL на безопасность
     * @param url URL для проверки
     * @return true если запрос отправлен успешно
     */
    bool checkUrl(const QUrl &url);

    /**
     * @brief Проверка URL на безопасность (синхронная)
     * @param url URL для проверки
     * @return Результат проверки
     */
    UrlCheckResponse checkUrlSync(const QUrl &url);

    /**
     * @brief Получение категории сайта
     * @param url URL для проверки
     * @return Категория сайта
     */
    SiteCategory getSiteCategory(const QUrl &url);

    /**
     * @brief Проверка родительского контроля
     * @param url URL для проверки
     * @param profileId ID профиля пользователя
     * @return true если доступ разрешен
     */
    bool checkParentalControl(const QUrl &url, const QString &profileId);

    /**
     * @brief Очистка кэша
     */
    void clearCache();

    /**
     * @brief Получение статистики
     * @return JSON объект со статистикой
     */
    QJsonObject getStatistics() const;

    /**
     * @brief Проверка подключения к API
     * @return true если подключение активно
     */
    bool isConnected() const;

    /**
     * @brief Установка API ключа
     * @param apiKey API ключ
     */
    void setApiKey(const QString &apiKey);

    /**
     * @brief Получение API ключа
     * @return API ключ
     */
    QString getApiKey() const;

    /**
     * @brief Установка таймаута
     * @param timeoutMs Таймаут в миллисекундах
     */
    void setTimeout(int timeoutMs);

    /**
     * @brief Получение таймаута
     * @return Таймаут в миллисекундах
     */
    int getTimeout() const;

signals:
    /**
     * @brief Сигнал о завершении проверки URL
     * @param url Проверенный URL
     * @param response Результат проверки
     */
    void urlChecked(const QUrl &url, const UrlCheckResponse &response);

    /**
     * @brief Сигнал об ошибке
     * @param error Текст ошибки
     */
    void errorOccurred(const QString &error);

    /**
     * @brief Сигнал об изменении статуса подключения
     * @param connected true если подключен
     */
    void connectionStatusChanged(bool connected);

    /**
     * @brief Сигнал об обновлении статистики
     * @param statistics Статистика
     */
    void statisticsUpdated(const QJsonObject &statistics);

private slots:
    /**
     * @brief Обработка ответа от API
     */
    void onReplyFinished();

    /**
     * @brief Обработка ошибки сети
     */
    void onNetworkError(QNetworkReply::NetworkError error);

    /**
     * @brief Обработка таймаута
     */
    void onTimeout();

    /**
     * @brief Обработка очереди запросов
     */
    void processQueue();

    /**
     * @brief Очистка устаревших записей кэша
     */
    void cleanupCache();

private:
    /**
     * @brief Отправка запроса к API
     * @param url URL для проверки
     * @return true если запрос отправлен
     */
    bool sendRequest(const QUrl &url);

    /**
     * @brief Обработка ответа API
     * @param reply Ответ от сервера
     * @return Результат проверки
     */
    UrlCheckResponse parseResponse(QNetworkReply *reply);

    /**
     * @brief Получение результата из кэша
     * @param url URL для поиска
     * @return Результат из кэша или пустой результат
     */
    UrlCheckResponse getCachedResult(const QUrl &url) const;

    /**
     * @brief Сохранение результата в кэш
     * @param url URL
     * @param response Результат проверки
     */
    void cacheResult(const QUrl &url, const UrlCheckResponse &response);

    /**
     * @brief Проверка валидности API ключа
     * @return true если ключ валиден
     */
    bool validateApiKey() const;

    /**
     * @brief Логирование запроса
     * @param url URL
     * @param response Результат
     */
    void logRequest(const QUrl &url, const UrlCheckResponse &response);

    /**
     * @brief Обновление статистики
     */
    void updateStatistics();

private:
    Config m_config;                                    ///< Конфигурация клиента
    QNetworkAccessManager *m_networkManager;            ///< Менеджер сети
    QQueue<QUrl> m_requestQueue;                        ///< Очередь запросов
    QMutex m_queueMutex;                                ///< Мьютекс для очереди
    QTimer *m_queueTimer;                               ///< Таймер обработки очереди
    QTimer *m_cacheTimer;                               ///< Таймер очистки кэша
    QHash<QString, UrlCheckResponse> m_cache;           ///< Кэш результатов
    QMutex m_cacheMutex;                                ///< Мьютекс для кэша
    QAtomicInt m_activeRequests;                        ///< Количество активных запросов
    QAtomicInt m_totalRequests;                         ///< Общее количество запросов
    QAtomicInt m_cachedRequests;                        ///< Количество запросов из кэша
    QAtomicInt m_errorRequests;                         ///< Количество ошибочных запросов
    QDateTime m_lastRequestTime;                        ///< Время последнего запроса
    bool m_connected;                                   ///< Статус подключения
    QMutex m_connectionMutex;                           ///< Мьютекс для статуса подключения
};

#endif // KSNCLIENT_H