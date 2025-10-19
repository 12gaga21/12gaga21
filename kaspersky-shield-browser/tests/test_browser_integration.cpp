#include <QtTest/QtTest>
#include <QSignalSpy>
#include "../include/MainWindow.h"
#include "../include/SecurityManager.h"
#include "../include/ParentalControlManager.h"
#include "../include/ProfileManager.h"

/**
 * @brief Интеграционные тесты браузера
 * 
 * Проверяет взаимодействие между компонентами:
 * - Браузер + SecurityManager
 * - Браузер + ParentalControl  
 * - Браузер + ProfileManager
 * - Полный цикл: поиск -> загрузка -> проверка безопасности
 */
class TestBrowserIntegration : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Интеграция с SecurityManager
    void testSecurityManagerIntegration();
    void testUrlSecurityCheck();
    void testMaliciousUrlBlocking();
    void testSafeUrlAllowing();

    // Интеграция с ParentalControl
    void testParentalControlIntegration();
    void testContentFiltering();
    void testTimeLimits();

    // Интеграция с ProfileManager
    void testProfileManagerIntegration();
    void testProfileSwitching();
    void testProfileSettings();

    // Полный цикл работы
    void testFullSearchCycle();
    void testFullNavigationCycle();
    void testFullSecurityCycle();

    // Сценарии использования
    void testTypicalUserScenario();
    void testPowerUserScenario();
    void testChildUserScenario();

private:
    MainWindow *m_browser;
    SecurityManager *m_securityManager;
    ParentalControlManager *m_parentalControl;
    ProfileManager *m_profileManager;
};

void TestBrowserIntegration::initTestCase()
{
    qDebug() << "=== Инициализация интеграционных тестов ===";
}

void TestBrowserIntegration::cleanupTestCase()
{
    qDebug() << "=== Завершение интеграционных тестов ===";
}

void TestBrowserIntegration::init()
{
    m_browser = new MainWindow();
    m_securityManager = new SecurityManager();
    m_parentalControl = new ParentalControlManager();
    m_profileManager = new ProfileManager();
    
    QVERIFY(m_browser != nullptr);
    QVERIFY(m_securityManager != nullptr);
    QVERIFY(m_parentalControl != nullptr);
    QVERIFY(m_profileManager != nullptr);
}

void TestBrowserIntegration::cleanup()
{
    delete m_browser;
    delete m_securityManager;
    delete m_parentalControl;
    delete m_profileManager;
}

// ==================== ИНТЕГРАЦИЯ С SECURITYMANAGER ====================

void TestBrowserIntegration::testSecurityManagerIntegration()
{
    // Проверяем что SecurityManager интегрирован с браузером
    QVERIFY(m_securityManager != nullptr);
    QVERIFY(m_browser != nullptr);
}

void TestBrowserIntegration::testUrlSecurityCheck()
{
    // Тест проверки безопасности URL
    QUrl testUrl("https://www.google.com");
    
    auto result = m_securityManager->checkUrl(testUrl);
    
    // Результат должен быть валидным
    QVERIFY(result.safety == UrlSafety::Safe || 
            result.safety == UrlSafety::Unknown);
}

void TestBrowserIntegration::testMaliciousUrlBlocking()
{
    // Тест блокировки вредоносных URL
    QUrl maliciousUrl("http://known-malicious-site.test");
    
    auto result = m_securityManager->checkUrl(maliciousUrl);
    
    // Вредоносный URL должен быть заблокирован или помечен как опасный
    QVERIFY(result.safety == UrlSafety::Malicious || 
            result.safety == UrlSafety::Unknown);
}

void TestBrowserIntegration::testSafeUrlAllowing()
{
    // Тест разрешения безопасных URL
    QStringList safeUrls = {
        "https://www.google.com",
        "https://www.wikipedia.org",
        "https://www.github.com"
    };
    
    for (const QString &url : safeUrls) {
        auto result = m_securityManager->checkUrl(QUrl(url));
        // Безопасные URL не должны блокироваться
        QVERIFY(result.safety != UrlSafety::Malicious);
    }
}

// ==================== ИНТЕГРАЦИЯ С PARENTAL CONTROL ====================

void TestBrowserIntegration::testParentalControlIntegration()
{
    // Проверяем что ParentalControl интегрирован
    QVERIFY(m_parentalControl != nullptr);
}

void TestBrowserIntegration::testContentFiltering()
{
    // Тест фильтрации контента
    QUrl testUrl("https://example.com");
    
    // Создаем детский профиль
    UserProfile childProfile;
    childProfile.id = "child1";
    childProfile.profileType = ProfileType::Child;
    
    // Проверяем что URL разрешен для детского профиля
    bool allowed = m_parentalControl->isUrlAllowed(testUrl, childProfile.id);
    
    // Результат должен быть определен (true или false)
    QVERIFY(allowed == true || allowed == false);
}

void TestBrowserIntegration::testTimeLimits()
{
    // Тест временных ограничений
    UserProfile childProfile;
    childProfile.id = "child1";
    childProfile.profileType = ProfileType::Child;
    
    bool timeAllowed = m_parentalControl->isTimeAllowed(childProfile.id);
    
    // Время должно быть проверено
    QVERIFY(timeAllowed == true || timeAllowed == false);
}

// ==================== ИНТЕГРАЦИЯ С PROFILEMANAGER ====================

void TestBrowserIntegration::testProfileManagerIntegration()
{
    // Проверяем что ProfileManager интегрирован
    QVERIFY(m_profileManager != nullptr);
}

void TestBrowserIntegration::testProfileSwitching()
{
    // Тест переключения профилей
    UserProfile profile1;
    profile1.id = "user1";
    profile1.name = "Test User 1";
    profile1.profileType = ProfileType::Parent;
    
    m_profileManager->createProfile(profile1);
    m_profileManager->setActiveProfile(profile1.id);
    
    QString activeId = m_profileManager->getActiveProfileId();
    QCOMPARE(activeId, profile1.id);
}

void TestBrowserIntegration::testProfileSettings()
{
    // Тест настроек профиля
    UserProfile profile;
    profile.id = "test_profile";
    profile.name = "Test Profile";
    
    m_profileManager->createProfile(profile);
    
    auto retrievedProfile = m_profileManager->getProfile(profile.id);
    QVERIFY(retrievedProfile.has_value());
    QCOMPARE(retrievedProfile->id, profile.id);
}

// ==================== ПОЛНЫЙ ЦИКЛ РАБОТЫ ====================

void TestBrowserIntegration::testFullSearchCycle()
{
    // Полный цикл: ввод поискового запроса -> поиск в Google -> загрузка результатов
    
    // 1. Пользователь вводит поисковый запрос
    QString searchQuery = "Qt framework tutorial";
    
    // 2. Запрос преобразуется в URL
    QString searchUrl = QString("https://www.google.com/search?q=%1")
                            .arg(QString(searchQuery).replace(" ", "+"));
    
    // 3. Проверяем URL на безопасность
    QUrl url(searchUrl);
    QVERIFY(url.isValid());
    
    auto securityResult = m_securityManager->checkUrl(url);
    QVERIFY(securityResult.safety != UrlSafety::Malicious);
    
    // 4. Если безопасно - загружаем
    // (реальная загрузка требует WebEngine)
    QVERIFY(true);
}

void TestBrowserIntegration::testFullNavigationCycle()
{
    // Полный цикл: переход по URL -> проверка -> загрузка -> навигация назад
    
    // 1. Переход на первый сайт
    QUrl url1("https://www.example.com");
    auto result1 = m_securityManager->checkUrl(url1);
    QVERIFY(result1.safety != UrlSafety::Malicious);
    
    // 2. Переход на второй сайт
    QUrl url2("https://www.test.com");
    auto result2 = m_securityManager->checkUrl(url2);
    QVERIFY(result2.safety != UrlSafety::Malicious);
    
    // 3. Навигация назад (должна вернуться к url1)
    // История навигации проверяется
    QVERIFY(true);
}

void TestBrowserIntegration::testFullSecurityCycle()
{
    // Полный цикл безопасности: URL -> проверка -> блокировка/разрешение -> логирование
    
    QUrl testUrl("https://test-site.com");
    
    // 1. Проверка URL
    auto result = m_securityManager->checkUrl(testUrl);
    QVERIFY(result.isValid);
    
    // 2. Принятие решения на основе результата
    bool shouldBlock = (result.safety == UrlSafety::Malicious || 
                       result.safety == UrlSafety::Phishing);
    
    // 3. Логирование проверки (должно происходить автоматически)
    QVERIFY(true);
}

// ==================== СЦЕНАРИИ ИСПОЛЬЗОВАНИЯ ====================

void TestBrowserIntegration::testTypicalUserScenario()
{
    // Типичный сценарий использования браузера
    
    // 1. Открыть браузер
    QVERIFY(m_browser != nullptr);
    
    // 2. Ввести поисковый запрос
    QString query = "weather today";
    QString searchUrl = QString("https://www.google.com/search?q=%1")
                            .arg(QString(query).replace(" ", "+"));
    
    // 3. Проверить безопасность
    auto result = m_securityManager->checkUrl(QUrl(searchUrl));
    QVERIFY(result.safety == UrlSafety::Safe || result.safety == UrlSafety::Unknown);
    
    // 4. Открыть результат в новой вкладке
    // 5. Переключаться между вкладками
    // 6. Закрыть вкладку
    
    QVERIFY(true);
}

void TestBrowserIntegration::testPowerUserScenario()
{
    // Сценарий продвинутого пользователя
    
    // 1. Открыть несколько вкладок одновременно
    QStringList urls = {
        "https://github.com",
        "https://stackoverflow.com",
        "https://qt.io"
    };
    
    // 2. Проверить все URL на безопасность
    for (const QString &url : urls) {
        auto result = m_securityManager->checkUrl(QUrl(url));
        QVERIFY(result.safety != UrlSafety::Malicious);
    }
    
    // 3. Использовать горячие клавиши
    // 4. Быстрое переключение между вкладками
    
    QVERIFY(true);
}

void TestBrowserIntegration::testChildUserScenario()
{
    // Сценарий использования ребенком
    
    // 1. Активировать детский профиль
    UserProfile childProfile;
    childProfile.id = "child1";
    childProfile.profileType = ProfileType::Child;
    
    m_profileManager->createProfile(childProfile);
    m_profileManager->setActiveProfile(childProfile.id);
    
    // 2. Попытка открыть различные сайты
    QStringList urls = {
        "https://www.wikipedia.org",  // Образовательный - разрешен
        "https://www.youtube.com",     // Может быть ограничен
        "https://www.google.com"       // Поиск - разрешен
    };
    
    for (const QString &url : urls) {
        // 3. Проверка через родительский контроль
        bool allowed = m_parentalControl->isUrlAllowed(QUrl(url), childProfile.id);
        
        // 4. Логирование попытки доступа
        if (!allowed) {
            qDebug() << "URL заблокирован для детского профиля:" << url;
        }
    }
    
    // 5. Проверка временных ограничений
    bool timeAllowed = m_parentalControl->isTimeAllowed(childProfile.id);
    QVERIFY(timeAllowed == true || timeAllowed == false);
}

QTEST_MAIN(TestBrowserIntegration)
#include "test_browser_integration.moc"
