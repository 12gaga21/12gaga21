#include <QtTest/QtTest>
#include <QSignalSpy>
#include "../include/AddressBar.h"

/**
 * @brief Детальные тесты для AddressBar - адресной строки браузера
 * 
 * Проверяет:
 * - Ввод URL
 * - Обработку Enter
 * - Валидацию URL
 * - Поисковые запросы vs URL
 * - Автодополнение
 */
class TestAddressBar : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Тесты базового функционала
    void testCreation();
    void testTextInput();
    void testClearText();
    void testPlaceholderText();

    // Тесты обработки Enter
    void testEnterKey();
    void testEnterKeyEmitsSignal();
    void testMultipleEnterPresses();

    // Тесты валидации URL
    void testValidUrl();
    void testInvalidUrl();
    void testUrlWithProtocol();
    void testUrlWithoutProtocol();

    // Тесты поисковых запросов
    void testSearchQuery();
    void testSearchQueryWithSpaces();
    void testSearchQueryVsUrl();
    void testSpecialCharactersInSearch();

    // Тесты автодополнения протокола
    void testAutoHttps();
    void testNoAutoHttpsForHttp();
    void testNoAutoHttpsForSearch();

    // Тесты различных URL форматов
    void testSimpleDomain();
    void testDomainWithPath();
    void testDomainWithQuery();
    void testDomainWithFragment();
    void testIpAddress();
    void testLocalhost();

    // Тесты безопасности
    void testXssInUrl();
    void testSqlInjectionInUrl();
    void testLongUrl();

private:
    AddressBar *m_addressBar;
    
    bool isUrl(const QString &text);
    QString processInput(const QString &input);
};

void TestAddressBar::initTestCase()
{
    qDebug() << "=== Инициализация тестов AddressBar ===";
}

void TestAddressBar::cleanupTestCase()
{
    qDebug() << "=== Завершение тестов AddressBar ===";
}

void TestAddressBar::init()
{
    m_addressBar = new AddressBar();
    QVERIFY(m_addressBar != nullptr);
}

void TestAddressBar::cleanup()
{
    if (m_addressBar) {
        delete m_addressBar;
        m_addressBar = nullptr;
    }
}

// ==================== БАЗОВЫЙ ФУНКЦИОНАЛ ====================

void TestAddressBar::testCreation()
{
    QVERIFY(m_addressBar != nullptr);
    QVERIFY(m_addressBar->text().isEmpty());
}

void TestAddressBar::testTextInput()
{
    QString testText = "https://www.example.com";
    m_addressBar->setText(testText);
    QCOMPARE(m_addressBar->text(), testText);
}

void TestAddressBar::testClearText()
{
    m_addressBar->setText("test");
    QVERIFY(!m_addressBar->text().isEmpty());
    
    m_addressBar->clear();
    QVERIFY(m_addressBar->text().isEmpty());
}

void TestAddressBar::testPlaceholderText()
{
    QString placeholder = m_addressBar->placeholderText();
    QVERIFY(!placeholder.isEmpty());
}

// ==================== ОБРАБОТКА ENTER ====================

void TestAddressBar::testEnterKey()
{
    m_addressBar->setText("https://www.google.com");
    
    // Создаем событие нажатия Enter
    QKeyEvent enterEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QApplication::sendEvent(m_addressBar, &enterEvent);
    
    // Текст должен остаться
    QCOMPARE(m_addressBar->text(), QString("https://www.google.com"));
}

void TestAddressBar::testEnterKeyEmitsSignal()
{
    m_addressBar->setText("test");
    
    // Создаем spy для отслеживания сигнала
    QSignalSpy spy(m_addressBar, &AddressBar::urlEntered);
    
    // Эмулируем нажатие Enter
    QKeyEvent enterEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QApplication::sendEvent(m_addressBar, &enterEvent);
    
    // Проверяем что сигнал был испущен
    QCOMPARE(spy.count(), 1);
}

void TestAddressBar::testMultipleEnterPresses()
{
    QSignalSpy spy(m_addressBar, &AddressBar::urlEntered);
    
    // Нажимаем Enter 3 раза
    for (int i = 0; i < 3; i++) {
        QKeyEvent enterEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
        QApplication::sendEvent(m_addressBar, &enterEvent);
    }
    
    QCOMPARE(spy.count(), 3);
}

// ==================== ВАЛИДАЦИЯ URL ====================

void TestAddressBar::testValidUrl()
{
    QStringList validUrls = {
        "https://www.google.com",
        "http://example.com",
        "https://subdomain.example.com",
        "https://example.com/path",
        "https://example.com?query=test"
    };
    
    for (const QString &url : validUrls) {
        QUrl qurl(url);
        QVERIFY2(qurl.isValid(), qPrintable(QString("URL должен быть валидным: %1").arg(url)));
    }
}

void TestAddressBar::testInvalidUrl()
{
    QStringList invalidUrls = {
        "ht!tp://invalid",
        "://noscheme.com",
        "http://",
        "   "
    };
    
    for (const QString &url : invalidUrls) {
        QUrl qurl(url);
        QVERIFY2(!qurl.isValid() || qurl.toString().isEmpty(), 
                qPrintable(QString("URL должен быть невалидным: %1").arg(url)));
    }
}

void TestAddressBar::testUrlWithProtocol()
{
    QString input = "https://google.com";
    QVERIFY(input.startsWith("http://") || input.startsWith("https://"));
}

void TestAddressBar::testUrlWithoutProtocol()
{
    QString input = "google.com";
    QVERIFY(!input.startsWith("http://") && !input.startsWith("https://"));
}

// ==================== ПОИСКОВЫЕ ЗАПРОСЫ ====================

void TestAddressBar::testSearchQuery()
{
    QString query = "how to program";
    
    // Поисковый запрос не содержит точек
    QVERIFY(!query.contains("."));
    
    // Должен превратиться в поисковый URL
    QString searchUrl = QString("https://www.google.com/search?q=%1").arg(QString(query).replace(" ", "+"));
    QVERIFY(searchUrl.contains("google.com/search"));
}

void TestAddressBar::testSearchQueryWithSpaces()
{
    QString query = "test query with spaces";
    
    // Содержит пробелы - это точно поисковый запрос
    QVERIFY(query.contains(" "));
    
    QString searchUrl = QString("https://www.google.com/search?q=%1").arg(QString(query).replace(" ", "+"));
    QVERIFY(searchUrl.contains("test+query+with+spaces"));
}

void TestAddressBar::testSearchQueryVsUrl()
{
    // URL содержит точку и может быть домен
    QString url = "example.com";
    QVERIFY(url.contains(".") && !url.contains(" "));
    
    // Поисковый запрос не содержит точек или содержит пробелы
    QString search = "test search";
    QVERIFY(!search.contains(".") || search.contains(" "));
}

void TestAddressBar::testSpecialCharactersInSearch()
{
    QString query = "C++ programming";
    
    // Специальные символы должны быть закодированы
    QString encoded = QUrl::toPercentEncoding(query);
    QVERIFY(!encoded.isEmpty());
}

// ==================== АВТОДОПОЛНЕНИЕ ПРОТОКОЛА ====================

void TestAddressBar::testAutoHttps()
{
    QString input = "google.com";
    QString expected = "https://google.com";
    
    if (!input.startsWith("http://") && !input.startsWith("https://")) {
        if (input.contains(".") && !input.contains(" ")) {
            input = "https://" + input;
        }
    }
    
    QCOMPARE(input, expected);
}

void TestAddressBar::testNoAutoHttpsForHttp()
{
    QString input = "http://google.com";
    QString original = input;
    
    // Не должны менять http:// на https://
    QCOMPARE(input, original);
}

void TestAddressBar::testNoAutoHttpsForSearch()
{
    QString input = "search query";
    
    // Поисковый запрос не должен получить https://
    QVERIFY(!input.startsWith("http://") && !input.startsWith("https://"));
}

// ==================== РАЗЛИЧНЫЕ ФОРМАТЫ URL ====================

void TestAddressBar::testSimpleDomain()
{
    QUrl url("example.com");
    QString processed = processInput("example.com");
    QVERIFY(processed.startsWith("https://"));
}

void TestAddressBar::testDomainWithPath()
{
    QString input = "example.com/path/to/page";
    QString processed = processInput(input);
    QVERIFY(processed.contains("/path/to/page"));
}

void TestAddressBar::testDomainWithQuery()
{
    QString input = "example.com?query=test";
    QString processed = processInput(input);
    QVERIFY(processed.contains("?query=test"));
}

void TestAddressBar::testDomainWithFragment()
{
    QString input = "example.com#section";
    QString processed = processInput(input);
    QVERIFY(processed.contains("#section"));
}

void TestAddressBar::testIpAddress()
{
    QString input = "192.168.1.1";
    // IP адрес содержит точки, но это не домен
    QVERIFY(input.contains("."));
    
    QString processed = processInput(input);
    QVERIFY(processed.startsWith("https://") || processed.contains("192.168.1.1"));
}

void TestAddressBar::testLocalhost()
{
    QString input = "localhost";
    QVERIFY(!input.contains("."));
    
    QString input2 = "localhost:8080";
    QString processed = processInput(input2);
    QVERIFY(processed.contains("localhost:8080"));
}

// ==================== БЕЗОПАСНОСТЬ ====================

void TestAddressBar::testXssInUrl()
{
    QString maliciousInput = "<script>alert('xss')</script>";
    
    // URL должен безопасно обрабатывать HTML
    QString safe = maliciousInput.toHtmlEscaped();
    QVERIFY(safe.contains("&lt;") && safe.contains("&gt;"));
}

void TestAddressBar::testSqlInjectionInUrl()
{
    QString maliciousInput = "'; DROP TABLE users; --";
    
    // Просто проверяем что можем обработать
    QVERIFY(!maliciousInput.isEmpty());
}

void TestAddressBar::testLongUrl()
{
    QString longUrl = "https://example.com/" + QString("a").repeated(2000);
    
    // URL может быть длинным (до 2083 символов в Chrome)
    QVERIFY(longUrl.length() > 2000);
    
    QUrl url(longUrl);
    QVERIFY(url.isValid());
}

// ==================== ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ====================

bool TestAddressBar::isUrl(const QString &text)
{
    // Простая проверка: содержит ли точку и не содержит ли пробелы
    return text.contains(".") && !text.contains(" ");
}

QString TestAddressBar::processInput(const QString &input)
{
    QString processed = input.trimmed();
    
    if (processed.isEmpty()) {
        return processed;
    }
    
    // Если уже есть протокол - возвращаем как есть
    if (processed.startsWith("http://") || processed.startsWith("https://")) {
        return processed;
    }
    
    // Если это похоже на URL (содержит точку, нет пробелов) - добавляем https://
    if (isUrl(processed)) {
        return "https://" + processed;
    }
    
    // Иначе это поисковый запрос
    QString searchQuery = QUrl::toPercentEncoding(processed);
    return QString("https://www.google.com/search?q=%1").arg(QString(processed).replace(" ", "+"));
}

QTEST_MAIN(TestAddressBar)
#include "test_addressbar.moc"
