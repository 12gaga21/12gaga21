# API Документация - Kaspersky Home Shield Browser

## 📚 Обзор API

Данная документация описывает API компонентов Kaspersky Home Shield Browser. Все API спроектированы для обеспечения безопасности, родительского контроля и удобства использования.

## 🏗️ Структура API

### Основные модули

- **SecurityManager** - Управление безопасностью
- **ParentalControlManager** - Родительский контроль
- **DatabaseManager** - Управление данными
- **ProfileManager** - Управление профилями
- **KsnClient** - Клиент KSN API

## 🛡️ SecurityManager API

### Класс SecurityManager

**Назначение:** Управление безопасностью и проверка URL.

```cpp
class SecurityManager : public QObject
{
    Q_OBJECT
    
public:
    explicit SecurityManager(QObject *parent = nullptr);
    ~SecurityManager();
    
    // Основные методы
    void checkUrl(const QString &url);
    void checkUrlAsync(const QString &url);
    UrlSafety getUrlSafety(const QString &url) const;
    
    // Настройки
    void setKsnClient(KsnClient *client);
    void setDatabaseManager(DatabaseManager *manager);
    
signals:
    void urlChecked(const UrlCheckResult &result);
    void securityEvent(const SecurityEvent &event);
    void urlBlocked(const QString &url, const QString &reason);
    
private slots:
    void onUrlCheckCompleted(const UrlCheckResult &result);
    
private:
    KsnClient *m_ksnClient;
    DatabaseManager *m_databaseManager;
    QHash<QString, UrlCheckResult> m_cache;
};
```

### Методы SecurityManager

#### `checkUrl(const QString &url)`
Синхронная проверка URL на безопасность.

**Параметры:**
- `url` - URL для проверки

**Возвращает:** `void`

**Исключения:** Нет

**Пример использования:**
```cpp
SecurityManager securityManager;
securityManager.checkUrl("https://example.com");
```

#### `checkUrlAsync(const QString &url)`
Асинхронная проверка URL на безопасность.

**Параметры:**
- `url` - URL для проверки

**Возвращает:** `void`

**Сигналы:** `urlChecked(const UrlCheckResult &result)`

**Пример использования:**
```cpp
SecurityManager securityManager;
connect(&securityManager, &SecurityManager::urlChecked, 
        this, &MyClass::onUrlChecked);
securityManager.checkUrlAsync("https://example.com");
```

#### `getUrlSafety(const QString &url) const`
Получение статуса безопасности URL из кэша.

**Параметры:**
- `url` - URL для проверки

**Возвращает:** `UrlSafety` - статус безопасности

**Пример использования:**
```cpp
SecurityManager securityManager;
UrlSafety safety = securityManager.getUrlSafety("https://example.com");
if (safety == UrlSafety::Dangerous) {
    // Обработка опасного URL
}
```

## 👨‍👩‍👧‍👦 ParentalControlManager API

### Класс ParentalControlManager

**Назначение:** Родительский контроль и управление профилями.

```cpp
class ParentalControlManager : public QObject
{
    Q_OBJECT
    
public:
    explicit ParentalControlManager(QObject *parent = nullptr);
    ~ParentalControlManager();
    
    // Управление профилями
    bool createProfile(const UserProfile &profile);
    bool updateProfile(const UserProfile &profile);
    bool deleteProfile(const QString &profileId);
    QList<UserProfile> getProfiles() const;
    UserProfile getProfile(const QString &profileId) const;
    
    // Проверки доступа
    bool isTimeAllowed(const QString &profileId);
    bool isContentAllowed(const QString &profileId, const QUrl &url);
    bool isUrlBlocked(const QString &profileId, const QUrl &url);
    
    // Управление временем
    void updateUsageTime(const QString &profileId, int minutes);
    void resetDailyUsage(const QString &profileId);
    
    // Логирование
    void logActivity(const ActivityRecord &record);
    void logBlockedAccess(const QString &profileId, const QUrl &url, const QString &reason);
    
    // Настройки
    void setDatabaseManager(DatabaseManager *manager);
    void setProfileManager(ProfileManager *manager);
    
signals:
    void profileCreated(const QString &profileId);
    void profileUpdated(const QString &profileId);
    void profileDeleted(const QString &profileId);
    void timeLimitReached(const QString &profileId);
    void bedTimeReached(const QString &profileId);
    void urlBlocked(const QString &profileId, const QUrl &url, const QString &reason);
    
private:
    DatabaseManager *m_databaseManager;
    ProfileManager *m_profileManager;
    QHash<QString, UserProfile> m_profiles;
    QTimer *m_usageTimer;
};
```

### Методы ParentalControlManager

#### `createProfile(const UserProfile &profile)`
Создание нового профиля пользователя.

**Параметры:**
- `profile` - данные профиля

**Возвращает:** `bool` - успешность создания

**Сигналы:** `profileCreated(const QString &profileId)`

**Пример использования:**
```cpp
UserProfile profile;
profile.id = "child1";
profile.name = "Child User";
profile.type = ProfileType::Child;
profile.timeLimitMinutes = 60;

ParentalControlManager manager;
bool success = manager.createProfile(profile);
```

#### `isTimeAllowed(const QString &profileId)`
Проверка, разрешено ли время использования для профиля.

**Параметры:**
- `profileId` - ID профиля

**Возвращает:** `bool` - разрешено ли время

**Сигналы:** `timeLimitReached(const QString &profileId)`

**Пример использования:**
```cpp
ParentalControlManager manager;
if (!manager.isTimeAllowed("child1")) {
    // Время истекло
}
```

#### `isContentAllowed(const QString &profileId, const QUrl &url)`
Проверка, разрешен ли контент для профиля.

**Параметры:**
- `profileId` - ID профиля
- `url` - URL для проверки

**Возвращает:** `bool` - разрешен ли контент

**Пример использования:**
```cpp
ParentalControlManager manager;
QUrl url("https://example.com");
if (!manager.isContentAllowed("child1", url)) {
    // Контент заблокирован
}
```

## 💾 DatabaseManager API

### Класс DatabaseManager

**Назначение:** Управление базой данных и персистентностью.

```cpp
class DatabaseManager : public QObject
{
    Q_OBJECT
    
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();
    
    // Инициализация
    bool initializeDatabase();
    bool isInitialized() const;
    
    // Профили
    bool saveProfile(const UserProfile &profile);
    bool loadProfile(const QString &profileId, UserProfile &profile);
    bool deleteProfile(const QString &profileId);
    QList<UserProfile> loadAllProfiles();
    
    // События безопасности
    bool logSecurityEvent(const SecurityEvent &event);
    QList<SecurityEvent> getSecurityEvents(const QDateTime &from, const QDateTime &to);
    
    // История браузера
    bool logBrowsingHistory(const ActivityRecord &record);
    QList<ActivityRecord> getBrowsingHistory(const QString &profileId, const QDateTime &from, const QDateTime &to);
    
    // Настройки
    bool saveSetting(const QString &key, const QVariant &value);
    QVariant loadSetting(const QString &key, const QVariant &defaultValue = QVariant());
    
signals:
    void databaseError(const QString &error);
    void profileSaved(const QString &profileId);
    void securityEventLogged(const SecurityEvent &event);
    
private:
    QSqlDatabase m_database;
    bool m_initialized;
    
    bool createTables();
    bool createProfilesTable();
    bool createSecurityEventsTable();
    bool createBrowsingHistoryTable();
    bool createSettingsTable();
};
```

### Методы DatabaseManager

#### `initializeDatabase()`
Инициализация базы данных и создание таблиц.

**Параметры:** Нет

**Возвращает:** `bool` - успешность инициализации

**Исключения:** Нет

**Пример использования:**
```cpp
DatabaseManager dbManager;
if (!dbManager.initializeDatabase()) {
    qDebug() << "Failed to initialize database";
}
```

#### `saveProfile(const UserProfile &profile)`
Сохранение профиля в базе данных.

**Параметры:**
- `profile` - данные профиля

**Возвращает:** `bool` - успешность сохранения

**Сигналы:** `profileSaved(const QString &profileId)`

**Пример использования:**
```cpp
DatabaseManager dbManager;
UserProfile profile;
// ... заполнение профиля
bool success = dbManager.saveProfile(profile);
```

#### `logSecurityEvent(const SecurityEvent &event)`
Логирование события безопасности.

**Параметры:**
- `event` - событие безопасности

**Возвращает:** `bool` - успешность логирования

**Сигналы:** `securityEventLogged(const SecurityEvent &event)`

**Пример использования:**
```cpp
SecurityEvent event;
event.type = "URL_BLOCKED";
event.severity = "HIGH";
event.details = "Malicious website detected";
event.timestamp = QDateTime::currentDateTime();

DatabaseManager dbManager;
dbManager.logSecurityEvent(event);
```

## 👤 ProfileManager API

### Класс ProfileManager

**Назначение:** Управление профилями пользователей.

```cpp
class ProfileManager : public QObject
{
    Q_OBJECT
    
public:
    explicit ProfileManager(QObject *parent = nullptr);
    ~ProfileManager();
    
    // Управление профилями
    bool createProfile(const UserProfile &profile);
    bool updateProfile(const UserProfile &profile);
    bool deleteProfile(const QString &profileId);
    UserProfile getProfile(const QString &profileId) const;
    QList<UserProfile> getAllProfiles() const;
    
    // Активный профиль
    bool setActiveProfile(const QString &profileId);
    QString getActiveProfileId() const;
    UserProfile getActiveProfile() const;
    
    // Настройки
    void setDatabaseManager(DatabaseManager *manager);
    
signals:
    void profileCreated(const QString &profileId);
    void profileUpdated(const QString &profileId);
    void profileDeleted(const QString &profileId);
    void activeProfileChanged(const QString &profileId);
    
private:
    DatabaseManager *m_databaseManager;
    QString m_activeProfileId;
    QHash<QString, UserProfile> m_profiles;
};
```

### Методы ProfileManager

#### `createProfile(const UserProfile &profile)`
Создание нового профиля.

**Параметры:**
- `profile` - данные профиля

**Возвращает:** `bool` - успешность создания

**Сигналы:** `profileCreated(const QString &profileId)`

**Пример использования:**
```cpp
ProfileManager profileManager;
UserProfile profile;
profile.id = "parent1";
profile.name = "Parent User";
profile.type = ProfileType::Parent;

bool success = profileManager.createProfile(profile);
```

#### `setActiveProfile(const QString &profileId)`
Установка активного профиля.

**Параметры:**
- `profileId` - ID профиля

**Возвращает:** `bool` - успешность установки

**Сигналы:** `activeProfileChanged(const QString &profileId)`

**Пример использования:**
```cpp
ProfileManager profileManager;
bool success = profileManager.setActiveProfile("parent1");
```

## 🌐 KsnClient API

### Класс KsnClient

**Назначение:** Клиент для интеграции с Kaspersky Security Network.

```cpp
class KsnClient : public QObject
{
    Q_OBJECT
    
public:
    explicit KsnClient(QObject *parent = nullptr);
    ~KsnClient();
    
    // Проверка URL
    void checkUrlAsync(const QString &url);
    UrlCheckResult checkUrlSync(const QString &url);
    
    // Настройки
    void setApiKey(const QString &apiKey);
    void setBaseUrl(const QString &baseUrl);
    
signals:
    void urlChecked(const UrlCheckResult &result);
    void networkError(const QString &error);
    
private slots:
    void onNetworkReplyFinished();
    
private:
    QNetworkAccessManager *m_networkManager;
    QHash<QString, UrlCheckResult> m_cache;
    QString m_apiKey;
    QString m_baseUrl;
    
    UrlCheckResult parseResponse(const QByteArray &data);
    void cacheResult(const QString &url, const UrlCheckResult &result);
};
```

### Методы KsnClient

#### `checkUrlAsync(const QString &url)`
Асинхронная проверка URL через KSN API.

**Параметры:**
- `url` - URL для проверки

**Возвращает:** `void`

**Сигналы:** `urlChecked(const UrlCheckResult &result)`

**Пример использования:**
```cpp
KsnClient ksnClient;
connect(&ksnClient, &KsnClient::urlChecked, 
        this, &MyClass::onUrlChecked);
ksnClient.checkUrlAsync("https://example.com");
```

#### `checkUrlSync(const QString &url)`
Синхронная проверка URL через KSN API.

**Параметры:**
- `url` - URL для проверки

**Возвращает:** `UrlCheckResult` - результат проверки

**Пример использования:**
```cpp
KsnClient ksnClient;
UrlCheckResult result = ksnClient.checkUrlSync("https://example.com");
if (result.safety == UrlSafety::Dangerous) {
    // URL опасен
}
```

## 📊 Типы данных

### UserProfile

```cpp
struct UserProfile {
    QString id;                    // Уникальный ID профиля
    QString name;                  // Имя пользователя
    ProfileType type;              // Тип профиля (Parent/Child/Teen)
    int timeLimitMinutes;          // Лимит времени в минутах
    QTime bedTimeStart;            // Начало времени сна
    QTime bedTimeEnd;              // Конец времени сна
    QDateTime lastResetDate;       // Дата последнего сброса
    int currentUsageMinutes;       // Текущее использование в минутах
    bool isActive;                 // Активен ли профиль
    QString avatarPath;            // Путь к аватару
};
```

### UrlCheckResult

```cpp
struct UrlCheckResult {
    UrlSafety safety;              // Уровень безопасности
    QString threatType;            // Тип угрозы
    QString category;              // Категория контента
    int reputationScore;           // Репутационный балл
    bool shouldBlock;              // Следует ли блокировать
};
```

### SecurityEvent

```cpp
struct SecurityEvent {
    QString type;                  // Тип события
    QString severity;              // Серьезность
    QString details;               // Детали
    QDateTime timestamp;           // Временная метка
};
```

### ActivityRecord

```cpp
struct ActivityRecord {
    QString profileId;             // ID профиля
    QUrl url;                      // URL
    QString title;                 // Заголовок страницы
    QDateTime timestamp;           // Временная метка
    QString category;              // Категория контента
    bool wasBlocked;               // Был ли заблокирован
    QString blockReason;           // Причина блокировки
};
```

## 🔧 Примеры использования

### Базовый пример

```cpp
#include "SecurityManager.h"
#include "ParentalControlManager.h"
#include "DatabaseManager.h"

int main() {
    // Инициализация компонентов
    DatabaseManager dbManager;
    dbManager.initializeDatabase();
    
    SecurityManager securityManager;
    ParentalControlManager parentalControl;
    
    // Создание профиля
    UserProfile profile;
    profile.id = "child1";
    profile.name = "Child User";
    profile.type = ProfileType::Child;
    profile.timeLimitMinutes = 60;
    
    parentalControl.createProfile(profile);
    
    // Проверка URL
    securityManager.checkUrlAsync("https://example.com");
    
    return 0;
}
```

### Обработка событий

```cpp
class MyClass : public QObject
{
    Q_OBJECT
    
public slots:
    void onUrlChecked(const UrlCheckResult &result) {
        if (result.safety == UrlSafety::Dangerous) {
            qDebug() << "Dangerous URL detected:" << result.threatType;
        }
    }
    
    void onTimeLimitReached(const QString &profileId) {
        qDebug() << "Time limit reached for profile:" << profileId;
    }
};
```

---

**Версия API:** 1.0  
**Дата обновления:** 19 октября 2024  
**Статус:** Актуальная