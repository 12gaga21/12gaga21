#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMutex>
#include <QTimer>
#include <QLoggingCategory>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QThread>
#include <QAtomicInt>
#include <QQueue>
#include <QHash>

Q_DECLARE_LOGGING_CATEGORY(databaseManager)

/**
 * @brief DatabaseManager - Менеджер базы данных для профилей пользователей
 * 
 * Этот класс обеспечивает управление профилями пользователей,
 * настройками безопасности, историей браузера и родительским контролем.
 * 
 * Основные возможности:
 * - Управление профилями пользователей
 * - Настройки безопасности для каждого профиля
 * - Родительский контроль и ограничения
 * - История браузера и закладки
 * - Кэширование результатов
 * - Асинхронные операции
 * - Обработка ошибок и повторные попытки
 */
class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Структура профиля пользователя
     */
    struct UserProfile {
        QString id;                     ///< Уникальный ID профиля
        QString name;                   ///< Имя профиля
        QString description;            ///< Описание профиля
        QString avatar;                 ///< Путь к аватару
        QDateTime created;              ///< Дата создания
        QDateTime lastUsed;             ///< Дата последнего использования
        bool isActive;                  ///< Активен ли профиль
        QJsonObject settings;           ///< Настройки профиля
        QJsonObject parentalControl;    ///< Настройки родительского контроля
        QJsonObject securitySettings;   ///< Настройки безопасности
        QJsonObject browserSettings;    ///< Настройки браузера
    };

    /**
     * @brief Структура записи истории
     */
    struct HistoryEntry {
        QString id;                     ///< Уникальный ID записи
        QString profileId;              ///< ID профиля
        QString url;                    ///< URL страницы
        QString title;                  ///< Заголовок страницы
        QDateTime visitTime;            ///< Время посещения
        int visitCount;                 ///< Количество посещений
        QJsonObject metadata;           ///< Дополнительные данные
    };

    /**
     * @brief Структура закладки
     */
    struct Bookmark {
        QString id;                     ///< Уникальный ID закладки
        QString profileId;              ///< ID профиля
        QString url;                    ///< URL страницы
        QString title;                  ///< Заголовок закладки
        QString description;            ///< Описание закладки
        QString folder;                 ///< Папка закладки
        QDateTime created;              ///< Дата создания
        QDateTime modified;             ///< Дата изменения
        QJsonObject metadata;           ///< Дополнительные данные
    };

    /**
     * @brief Конфигурация базы данных
     */
    struct Config {
        QString databasePath;           ///< Путь к файлу базы данных
        int maxConnections;             ///< Максимальное количество соединений
        int connectionTimeout;          ///< Таймаут соединения в секундах
        int queryTimeout;               ///< Таймаут запроса в секундах
        bool enableLogging;             ///< Включить логирование
        bool enableBackup;              ///< Включить резервное копирование
        int backupInterval;             ///< Интервал резервного копирования в часах
        bool enableCompression;         ///< Включить сжатие базы данных
        bool enableEncryption;          ///< Включить шифрование базы данных
    };

    /**
     * @brief Конструктор
     * @param parent Родительский объект
     */
    explicit DatabaseManager(QObject *parent = nullptr);

    /**
     * @brief Деструктор
     */
    ~DatabaseManager();

    /**
     * @brief Инициализация менеджера базы данных
     * @param config Конфигурация
     * @return true если инициализация успешна
     */
    bool initialize(const Config &config);

    /**
     * @brief Закрытие соединения с базой данных
     */
    void close();

    /**
     * @brief Проверка подключения к базе данных
     * @return true если подключен
     */
    bool isConnected() const;

    // Управление профилями
    /**
     * @brief Создание нового профиля
     * @param profile Данные профиля
     * @return true если создание успешно
     */
    bool createProfile(const UserProfile &profile);

    /**
     * @brief Обновление профиля
     * @param profile Данные профиля
     * @return true если обновление успешно
     */
    bool updateProfile(const UserProfile &profile);

    /**
     * @brief Удаление профиля
     * @param profileId ID профиля
     * @return true если удаление успешно
     */
    bool deleteProfile(const QString &profileId);

    /**
     * @brief Получение профиля по ID
     * @param profileId ID профиля
     * @return Профиль или пустой профиль
     */
    UserProfile getProfile(const QString &profileId) const;

    /**
     * @brief Получение всех профилей
     * @return Список профилей
     */
    QList<UserProfile> getAllProfiles() const;

    /**
     * @brief Получение активного профиля
     * @return Активный профиль или пустой профиль
     */
    UserProfile getActiveProfile() const;

    /**
     * @brief Установка активного профиля
     * @param profileId ID профиля
     * @return true если установка успешна
     */
    bool setActiveProfile(const QString &profileId);

    // Управление историей
    /**
     * @brief Добавление записи в историю
     * @param entry Запись истории
     * @return true если добавление успешно
     */
    bool addHistoryEntry(const HistoryEntry &entry);

    /**
     * @brief Получение истории профиля
     * @param profileId ID профиля
     * @param limit Ограничение количества записей
     * @return Список записей истории
     */
    QList<HistoryEntry> getHistory(const QString &profileId, int limit = 100) const;

    /**
     * @brief Очистка истории профиля
     * @param profileId ID профиля
     * @return true если очистка успешна
     */
    bool clearHistory(const QString &profileId);

    /**
     * @brief Удаление старых записей истории
     * @param days Количество дней
     * @return true если удаление успешно
     */
    bool cleanupHistory(int days = 30);

    // Управление закладками
    /**
     * @brief Добавление закладки
     * @param bookmark Закладка
     * @return true если добавление успешно
     */
    bool addBookmark(const Bookmark &bookmark);

    /**
     * @brief Обновление закладки
     * @param bookmark Закладка
     * @return true если обновление успешно
     */
    bool updateBookmark(const Bookmark &bookmark);

    /**
     * @brief Удаление закладки
     * @param bookmarkId ID закладки
     * @return true если удаление успешно
     */
    bool deleteBookmark(const QString &bookmarkId);

    /**
     * @brief Получение закладок профиля
     * @param profileId ID профиля
     * @return Список закладок
     */
    QList<Bookmark> getBookmarks(const QString &profileId) const;

    /**
     * @brief Получение закладок по папке
     * @param profileId ID профиля
     * @param folder Папка
     * @return Список закладок
     */
    QList<Bookmark> getBookmarksByFolder(const QString &profileId, const QString &folder) const;

    // Резервное копирование
    /**
     * @brief Создание резервной копии
     * @param backupPath Путь к резервной копии
     * @return true если копирование успешно
     */
    bool createBackup(const QString &backupPath) const;

    /**
     * @brief Восстановление из резервной копии
     * @param backupPath Путь к резервной копии
     * @return true если восстановление успешно
     */
    bool restoreBackup(const QString &backupPath);

    /**
     * @brief Получение статистики базы данных
     * @return JSON объект со статистикой
     */
    QJsonObject getStatistics() const;

    /**
     * @brief Оптимизация базы данных
     * @return true если оптимизация успешна
     */
    bool optimizeDatabase();

    /**
     * @brief Проверка целостности базы данных
     * @return true если база данных целостна
     */
    bool checkIntegrity() const;

signals:
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

    /**
     * @brief Сигнал о создании профиля
     * @param profileId ID созданного профиля
     */
    void profileCreated(const QString &profileId);

    /**
     * @brief Сигнал об обновлении профиля
     * @param profileId ID обновленного профиля
     */
    void profileUpdated(const QString &profileId);

    /**
     * @brief Сигнал об удалении профиля
     * @param profileId ID удаленного профиля
     */
    void profileDeleted(const QString &profileId);

    /**
     * @brief Сигнал о добавлении записи истории
     * @param profileId ID профиля
     * @param entry Запись истории
     */
    void historyEntryAdded(const QString &profileId, const HistoryEntry &entry);

    /**
     * @brief Сигнал о добавлении закладки
     * @param profileId ID профиля
     * @param bookmark Закладка
     */
    void bookmarkAdded(const QString &profileId, const Bookmark &bookmark);

private slots:
    /**
     * @brief Обработка резервного копирования
     */
    void onBackupTimer();

    /**
     * @brief Обработка оптимизации базы данных
     */
    void onOptimizeTimer();

private:
    /**
     * @brief Создание таблиц базы данных
     * @return true если создание успешно
     */
    bool createTables();

    /**
     * @brief Создание индексов
     * @return true если создание успешно
     */
    bool createIndexes();

    /**
     * @brief Выполнение SQL запроса
     * @param query SQL запрос
     * @return true если выполнение успешно
     */
    bool executeQuery(const QString &query) const;

    /**
     * @brief Выполнение SQL запроса с параметрами
     * @param query SQL запрос
     * @param params Параметры запроса
     * @return true если выполнение успешно
     */
    bool executeQuery(const QString &query, const QVariantList &params) const;

    /**
     * @brief Получение соединения с базой данных
     * @return Соединение с базой данных
     */
    QSqlDatabase getDatabase() const;

    /**
     * @brief Логирование операции
     * @param operation Название операции
     * @param details Детали операции
     */
    void logOperation(const QString &operation, const QString &details) const;

    /**
     * @brief Обновление статистики
     */
    void updateStatistics();

private:
    Config m_config;                                    ///< Конфигурация
    QSqlDatabase m_database;                            ///< База данных
    mutable QMutex m_mutex;                             ///< Мьютекс для синхронизации
    QTimer *m_backupTimer;                              ///< Таймер резервного копирования
    QTimer *m_optimizeTimer;                            ///< Таймер оптимизации
    QAtomicInt m_operationCount;                        ///< Счетчик операций
    QAtomicInt m_errorCount;                            ///< Счетчик ошибок
    QDateTime m_lastBackup;                             ///< Время последнего резервного копирования
    QDateTime m_lastOptimize;                           ///< Время последней оптимизации
    bool m_connected;                                   ///< Статус подключения
    QHash<QString, QDateTime> m_lastAccess;             ///< Время последнего доступа к таблицам
};

#endif // DATABASEMANAGER_H