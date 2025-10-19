#include "MainWindow.h"
#include "AddressBar.h"
#include "SecurityManager.h"
#include "ParentalControlManager.h"
#include "ProfileManager.h"
#include "KsnClient.h"
#include "DatabaseManager.h"
#include "SecuritySettingsDialog.h"
#include <QApplication>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QTabWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QStatusBar>
#include <QMenuBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QNetworkReply>
#include <QUrl>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_tabWidget(nullptr)
    , m_currentWebView(nullptr)
    , m_addressBar(nullptr)
    , m_backButton(nullptr)
    , m_forwardButton(nullptr)
    , m_refreshButton(nullptr)
    , m_stopButton(nullptr)
    , m_securityButton(nullptr)
    , m_parentalControlButton(nullptr)
    , m_profileButton(nullptr)
    , m_newTabButton(nullptr)
    , m_progressBar(nullptr)
    , m_statusLabel(nullptr)
    , m_ksnClient(nullptr)
    , m_databaseManager(nullptr)
    , m_securityManager(nullptr)
    , m_parentalControlManager(nullptr)
    , m_profileManager(nullptr)
    , m_statusTimer(nullptr)
    , m_webProfile(nullptr)
    , m_isLoading(false)
{
    // Initialize all components first
    initializeComponents();
    
    // Setup UI
    setupWebEngine();
    setupUI();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupConnections();
    
    // Connect components
    connectComponents();
    
    // Set window properties
    setWindowTitle("Kaspersky Home Shield Browser - Защищенный браузер");
    setMinimumSize(1024, 768);
    resize(1200, 800);
    
    // Load initial page
    createNewTab("https://www.kaspersky.com");
    
    qDebug() << "MainWindow initialized successfully";
}

MainWindow::~MainWindow()
{
    // Cleanup is handled by Qt's parent-child system
}

void MainWindow::setupWebEngine()
{
    // Create web profile with security settings
    m_webProfile = QWebEngineProfile::defaultProfile();
    
    // Configure security settings
    QWebEngineSettings *settings = m_webProfile->settings();
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::PluginsEnabled, false);
    settings->setAttribute(QWebEngineSettings::AutoLoadImages, true);
    settings->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, false);
    settings->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, false);
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, false);
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, false);
    settings->setAttribute(QWebEngineSettings::XSSAuditingEnabled, true);
    settings->setAttribute(QWebEngineSettings::ErrorPageEnabled, true);
    settings->setAttribute(QWebEngineSettings::WebGLEnabled, false);
    settings->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, false);
    settings->setAttribute(QWebEngineSettings::PrintElementBackgrounds, false);
    settings->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, false);
    settings->setAttribute(QWebEngineSettings::AllowGeolocationOnInsecureOrigins, false);
    settings->setAttribute(QWebEngineSettings::AllowWindowActivationFromJavaScript, false);
    settings->setAttribute(QWebEngineSettings::ShowScrollBars, true);
    settings->setAttribute(QWebEngineSettings::PlaybackRequiresUserGesture, true);
    settings->setAttribute(QWebEngineSettings::JavascriptCanPaste, false);
    settings->setAttribute(QWebEngineSettings::WebRTCPublicInterfacesOnly, true);
    settings->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, false);
    settings->setAttribute(QWebEngineSettings::PdfViewerEnabled, true);
    settings->setAttribute(QWebEngineSettings::ScreenCaptureEnabled, false);
    settings->setAttribute(QWebEngineSettings::AutoLoadIconsForPage, false);
    settings->setAttribute(QWebEngineSettings::TouchIconsEnabled, false);
    settings->setAttribute(QWebEngineSettings::FocusOnNavigationEnabled, true);
    settings->setAttribute(QWebEngineSettings::HyperlinkAuditingEnabled, false);
    settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
}

void MainWindow::setupUI()
{
    // Central widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Tab widget
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setMovable(true);
    m_tabWidget->setDocumentMode(true);
    mainLayout->addWidget(m_tabWidget);
    
    // Connect tab signals
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onCurrentTabChanged);
    connect(m_tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabCloseRequested);
}

void MainWindow::setupMenuBar()
{
    QMenuBar *menuBar = this->menuBar();
    
    // File menu
    QMenu *fileMenu = menuBar->addMenu("&File");
    QAction *newTabAction = fileMenu->addAction("&New Tab", this, &MainWindow::onNewTabRequested);
    newTabAction->setShortcut(QKeySequence::AddTab);
    
    QAction *closeTabAction = fileMenu->addAction("&Close Tab", this, [this]() {
        if (m_tabWidget->count() > 1) {
            closeTab(m_tabWidget->currentIndex());
        }
    });
    closeTabAction->setShortcut(QKeySequence::Close);
    
    fileMenu->addSeparator();
    
    QAction *exitAction = fileMenu->addAction("&Exit", this, &QWidget::close);
    exitAction->setShortcut(QKeySequence::Quit);
    
    // Edit menu
    QMenu *editMenu = menuBar->addMenu("&Edit");
    editMenu->addAction("&Find", this, [this]() {
        if (m_currentWebView) {
            m_currentWebView->findText("", QWebEnginePage::FindFlags());
        }
    }, QKeySequence::Find);
    editMenu->addAction("&Select All", this, [this]() {
        if (m_currentWebView) {
            m_currentWebView->triggerPageAction(QWebEnginePage::SelectAll);
        }
    }, QKeySequence::SelectAll);
    
    // View menu
    QMenu *viewMenu = menuBar->addMenu("&View");
    viewMenu->addAction("&Security Status", this, &MainWindow::showSecurityStatus);
    viewMenu->addAction("&Parental Control", this, &MainWindow::showParentalControl);
    viewMenu->addAction("&Profile Manager", this, &MainWindow::showProfileManager);
    viewMenu->addSeparator();
    viewMenu->addAction("&Zoom In", this, [this]() {
        if (m_currentWebView) {
            m_currentWebView->setZoomFactor(m_currentWebView->zoomFactor() + 0.1);
        }
    }, QKeySequence::ZoomIn);
    viewMenu->addAction("&Zoom Out", this, [this]() {
        if (m_currentWebView) {
            m_currentWebView->setZoomFactor(m_currentWebView->zoomFactor() - 0.1);
        }
    }, QKeySequence::ZoomOut);
    viewMenu->addAction("&Reset Zoom", this, [this]() {
        if (m_currentWebView) {
            m_currentWebView->setZoomFactor(1.0);
        }
    });
    
    // Help menu
    QMenu *helpMenu = menuBar->addMenu("&Help");
    helpMenu->addAction("&About", this, [this]() {
        QMessageBox::about(this, "About Kaspersky Home Shield Browser",
                          "Kaspersky Home Shield Browser v1.0.0\n"
                          "A secure browser with integrated Kaspersky protection.\n\n"
                          "Built with Qt WebEngine for full web compatibility.");
    });
}

void MainWindow::setupToolBar()
{
    QToolBar *toolBar = addToolBar("Main Toolbar");
    toolBar->setMovable(false);
    
    // Navigation buttons
    m_backButton = new QPushButton("←", this);
    m_backButton->setToolTip("Go Back");
    m_backButton->setEnabled(false);
    toolBar->addWidget(m_backButton);
    
    m_forwardButton = new QPushButton("→", this);
    m_forwardButton->setToolTip("Go Forward");
    m_forwardButton->setEnabled(false);
    toolBar->addWidget(m_forwardButton);
    
    m_refreshButton = new QPushButton("↻", this);
    m_refreshButton->setToolTip("Refresh");
    toolBar->addWidget(m_refreshButton);
    
    m_stopButton = new QPushButton("⏹", this);
    m_stopButton->setToolTip("Stop");
    m_stopButton->setVisible(false);
    toolBar->addWidget(m_stopButton);
    
    toolBar->addSeparator();
    
    // Address bar
    m_addressBar = new AddressBar(this);
    m_addressBar->setPlaceholderText("Enter URL or search term...");
    toolBar->addWidget(m_addressBar);
    
    toolBar->addSeparator();
    
    // New tab button
    m_newTabButton = new QPushButton("+", this);
    m_newTabButton->setToolTip("New Tab");
    toolBar->addWidget(m_newTabButton);
    
    toolBar->addSeparator();
    
    // Security and control buttons
    m_securityButton = new QPushButton("🛡️ Security", this);
    m_securityButton->setToolTip("Security Status");
    toolBar->addWidget(m_securityButton);
    
    m_parentalControlButton = new QPushButton("👨‍👩‍👧‍👦 Parental Control", this);
    m_parentalControlButton->setToolTip("Parental Control Settings");
    toolBar->addWidget(m_parentalControlButton);
    
    m_profileButton = new QPushButton("👤 Profile", this);
    m_profileButton->setToolTip("Profile Manager");
    toolBar->addWidget(m_profileButton);
}

void MainWindow::setupStatusBar()
{
    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    m_progressBar->setMaximumWidth(200);
    statusBar()->addWidget(m_progressBar);
    
    m_statusLabel = new QLabel("Ready", this);
    statusBar()->addWidget(m_statusLabel);
    
    m_statusTimer = new QTimer(this);
    m_statusTimer->setSingleShot(true);
    m_statusTimer->setInterval(3000);
    connect(m_statusTimer, &QTimer::timeout, [this]() {
        m_statusLabel->setText("Ready");
    });
}

void MainWindow::setupConnections()
{
    // Navigation buttons
    connect(m_backButton, &QPushButton::clicked, this, &MainWindow::goBack);
    connect(m_forwardButton, &QPushButton::clicked, this, &MainWindow::goForward);
    connect(m_refreshButton, &QPushButton::clicked, this, &MainWindow::refreshPage);
    connect(m_stopButton, &QPushButton::clicked, this, &MainWindow::stopLoading);
    
    // Address bar
    connect(m_addressBar, &AddressBar::urlEntered, this, &MainWindow::navigateToUrl);
    
    // Control buttons
    connect(m_securityButton, &QPushButton::clicked, this, &MainWindow::showSecurityStatus);
    connect(m_parentalControlButton, &QPushButton::clicked, this, &MainWindow::showParentalControl);
    connect(m_profileButton, &QPushButton::clicked, this, &MainWindow::showProfileManager);
    connect(m_newTabButton, &QPushButton::clicked, this, &MainWindow::onNewTabRequested);
}

QWebEngineView* MainWindow::createNewTab(const QString &url)
{
    QWebEngineView *webView = new QWebEngineView(this);
    webView->setPage(new QWebEnginePage(m_webProfile, webView));
    
    // Connect signals
    connect(webView, &QWebEngineView::urlChanged, this, &MainWindow::onUrlChanged);
    connect(webView, &QWebEngineView::titleChanged, this, &MainWindow::updateTitle);
    connect(webView, &QWebEngineView::loadStarted, this, &MainWindow::onLoadStarted);
    connect(webView, &QWebEngineView::loadFinished, this, &MainWindow::onLoadFinished);
    connect(webView, &QWebEngineView::loadProgress, this, &MainWindow::onLoadProgress);
    
    // Connect page signals
    QWebEnginePage *page = webView->page();
    // Note: Download and certificate error handling will be implemented later
    
    // Add to tab widget
    int index = m_tabWidget->addTab(webView, "New Tab");
    m_tabWidget->setCurrentIndex(index);
    m_currentWebView = webView;
    
    // Load URL if provided
    if (!url.isEmpty()) {
        webView->load(QUrl(url));
    } else {
        webView->load(QUrl("about:blank"));
    }
    
    return webView;
}

void MainWindow::closeTab(int index)
{
    if (m_tabWidget->count() > 1) {
        QWidget *widget = m_tabWidget->widget(index);
        m_tabWidget->removeTab(index);
        widget->deleteLater();
        
        // Update current web view
        if (m_tabWidget->count() > 0) {
            m_currentWebView = qobject_cast<QWebEngineView*>(m_tabWidget->currentWidget());
            updateNavigationButtons();
        }
    } else {
        QMessageBox::information(this, "Cannot Close Tab", 
                               "At least one tab must remain open.");
    }
}

void MainWindow::updateNavigationButtons()
{
    if (!m_currentWebView) return;
    
    QWebEngineHistory *history = m_currentWebView->history();
    m_backButton->setEnabled(history->canGoBack());
    m_forwardButton->setEnabled(history->canGoForward());
}

void MainWindow::checkUrlSecurity(const QUrl &url)
{
    if (!m_securityManager || !m_parentalControlManager || !m_profileManager) {
        return;
    }
    
    // Get current profile
    QString currentProfileId = m_profileManager->getCurrentProfileId();
    
    // Check URL security asynchronously
    m_securityManager->checkUrlAsync(url);
    
    // Check parental control
    if (!m_parentalControlManager->isUrlAllowed(url, currentProfileId)) {
        qDebug() << "URL blocked by parental control:" << url.toString();
        
        // Stop loading if currently loading
        if (m_currentWebView && m_isLoading) {
            m_currentWebView->stop();
        }
    }
}

void MainWindow::navigateToUrl()
{
    if (!m_currentWebView) return;
    
    QString url = m_addressBar->text().trimmed();
    if (url.isEmpty()) return;
    
    // Add protocol if missing
    if (!url.startsWith("http://") && !url.startsWith("https://") && !url.startsWith("file://") && !url.startsWith("about:")) {
        if (url.contains(".") && !url.contains(" ")) {
            url = "https://" + url;
        } else {
            url = "https://www.google.com/search?q=" + QUrl::toPercentEncoding(url);
        }
    }
    
    QUrl qurl(url);
    m_currentWebView->load(qurl);
    
    // Check URL security
    checkUrlSecurity(qurl);
}

void MainWindow::goBack()
{
    if (m_currentWebView && m_currentWebView->history()->canGoBack()) {
        m_currentWebView->back();
    }
}

void MainWindow::goForward()
{
    if (m_currentWebView && m_currentWebView->history()->canGoForward()) {
        m_currentWebView->forward();
    }
}

void MainWindow::refreshPage()
{
    if (m_currentWebView) {
        m_currentWebView->reload();
    }
}

void MainWindow::stopLoading()
{
    if (m_currentWebView) {
        m_currentWebView->stop();
    }
}

void MainWindow::updateUrlBar(const QUrl &url)
{
    if (m_addressBar) {
        m_addressBar->setText(url.toString());
    }
}

void MainWindow::updateTitle(const QString &title)
{
    if (!title.isEmpty()) {
        setWindowTitle(title + " - Kaspersky Home Shield Browser");
        int currentIndex = m_tabWidget->currentIndex();
        if (currentIndex >= 0) {
            QString shortTitle = title;
            if (shortTitle.length() > 20) {
                shortTitle = shortTitle.left(17) + "...";
            }
            m_tabWidget->setTabText(currentIndex, shortTitle);
        }
    }
}

void MainWindow::updateLoadingProgress(int progress)
{
    if (progress < 100) {
        m_progressBar->setVisible(true);
        m_progressBar->setValue(progress);
        m_stopButton->setVisible(true);
        m_refreshButton->setVisible(false);
    } else {
        m_progressBar->setVisible(false);
        m_stopButton->setVisible(false);
        m_refreshButton->setVisible(true);
    }
}

void MainWindow::showSecurityStatus()
{
    if (!m_securityManager) {
        QMessageBox::warning(this, "Ошибка", "SecurityManager не инициализирован");
        return;
    }
    
    SecuritySettingsDialog dialog(m_securityManager, this);
    dialog.exec();
}

void MainWindow::showParentalControl()
{
    QMessageBox::information(this, "Parental Control", 
                           "Parental control settings will be available here.\n"
                           "Profile management is ready.");
}

void MainWindow::showProfileManager()
{
    QMessageBox::information(this, "Profile Manager", 
                           "Profile management will be available here.\n"
                           "User profiles are ready for configuration.");
}

void MainWindow::onTabCloseRequested(int index)
{
    closeTab(index);
}

void MainWindow::onCurrentTabChanged(int index)
{
    if (index >= 0) {
        m_currentWebView = qobject_cast<QWebEngineView*>(m_tabWidget->widget(index));
        if (m_currentWebView) {
            updateUrlBar(m_currentWebView->url());
            updateTitle(m_currentWebView->title());
            updateNavigationButtons();
        }
    }
}

void MainWindow::onNewTabRequested()
{
    createNewTab();
}

void MainWindow::onDownloadRequested()
{
    // Handle download - will be implemented later
    m_statusLabel->setText("Download requested");
    m_statusTimer->start();
}

void MainWindow::onCertificateError()
{
    // Handle certificate errors - will be implemented later
    m_statusLabel->setText("Certificate error occurred");
    m_statusTimer->start();
}

void MainWindow::onUrlChanged(const QUrl &url)
{
    updateUrlBar(url);
    checkUrlSecurity(url);
}

void MainWindow::onLoadFinished(bool success)
{
    m_isLoading = false;
    updateLoadingProgress(100);
    
    if (success) {
        m_statusLabel->setText("Page loaded successfully");
    } else {
        m_statusLabel->setText("Failed to load page");
    }
    m_statusTimer->start();
}

void MainWindow::onLoadStarted()
{
    m_isLoading = true;
    m_statusLabel->setText("Loading...");
}

void MainWindow::onLoadProgress(int progress)
{
    updateLoadingProgress(progress);
}

void MainWindow::initializeComponents()
{
    qDebug() << "Initializing browser components...";
    
    // 1. Create KSN Client
    m_ksnClient = new KsnClient(this);
    KsnClient::Config ksnConfig;
    ksnConfig.apiKey = "demo_key"; // TODO: Get real API key
    ksnConfig.apiUrl = "https://api.kaspersky.com/ksn"; // Demo URL
    ksnConfig.timeoutMs = 5000;
    ksnConfig.maxRetries = 3;
    ksnConfig.cacheSize = 10000;
    ksnConfig.cacheExpiryHours = 24;
    ksnConfig.enableParentalControl = true;
    ksnConfig.enableRealTimeCheck = true;
    ksnConfig.enableCaching = true;
    ksnConfig.enableLogging = true;
    
    if (!m_ksnClient->initialize(ksnConfig)) {
        qWarning() << "Failed to initialize KSN Client - running in demo mode";
    } else {
        qDebug() << "KSN Client initialized successfully";
    }
    
    // 2. Create Database Manager
    m_databaseManager = new DatabaseManager(this);
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) 
                    + "/kaspersky_shield.db";
    
    if (!m_databaseManager->initialize(dbPath)) {
        qWarning() << "Failed to initialize Database Manager";
    } else {
        qDebug() << "Database Manager initialized successfully";
    }
    
    // 3. Create and initialize Security Manager
    m_securityManager = new SecurityManager(this);
    if (!m_securityManager->initialize(m_ksnClient)) {
        qWarning() << "Failed to initialize Security Manager";
    } else {
        qDebug() << "Security Manager initialized successfully";
    }
    
    // 4. Create and initialize Parental Control Manager
    m_parentalControlManager = new ParentalControlManager(this);
    if (!m_parentalControlManager->initialize(m_ksnClient)) {
        qWarning() << "Failed to initialize Parental Control Manager";
    } else {
        qDebug() << "Parental Control Manager initialized successfully";
    }
    
    // 5. Create and initialize Profile Manager
    m_profileManager = new ProfileManager(this);
    if (!m_profileManager->initialize(m_databaseManager)) {
        qWarning() << "Failed to initialize Profile Manager";
    } else {
        qDebug() << "Profile Manager initialized successfully";
    }
    
    qDebug() << "All components initialized";
}

void MainWindow::connectComponents()
{
    qDebug() << "Connecting component signals...";
    
    // Security Manager signals
    connect(m_securityManager, &SecurityManager::urlBlocked,
            this, [this](const QUrl &url, const QString &reason) {
        QMessageBox::warning(this, "URL Заблокирован",
                           QString("URL заблокирован по причине безопасности:\n\n%1\n\nПричина: %2")
                           .arg(url.toString()).arg(reason));
        m_statusLabel->setText(QString("Заблокирован: %1").arg(url.host()));
        m_statusTimer->start();
    });
    
    connect(m_securityManager, &SecurityManager::threatDetected,
            this, [this](const QUrl &url, const QString &threatType) {
        m_statusLabel->setText(QString("Угроза обнаружена: %1 - %2")
                              .arg(threatType).arg(url.host()));
        m_statusTimer->start();
    });
    
    // Parental Control signals
    connect(m_parentalControlManager, &ParentalControlManager::urlBlocked,
            this, [this](const QString &profileId, const QUrl &url, const QString &reason) {
        QMessageBox::information(this, "Родительский контроль",
                               QString("Доступ запрещен для профиля '%1':\n\n%2\n\nПричина: %3")
                               .arg(profileId).arg(url.toString()).arg(reason));
        m_statusLabel->setText(QString("Доступ запрещен: %1").arg(url.host()));
        m_statusTimer->start();
    });
    
    connect(m_parentalControlManager, &ParentalControlManager::timeLimitReached,
            this, [this](const QString &profileId) {
        QMessageBox::warning(this, "Лимит времени",
                           QString("Дневной лимит времени исчерпан для профиля '%1'")
                           .arg(profileId));
    });
    
    // Profile Manager signals
    connect(m_profileManager, &ProfileManager::currentProfileChanged,
            this, [this](const QString &profileId) {
        auto profile = m_profileManager->getProfile(profileId);
        setWindowTitle(QString("Kaspersky Home Shield Browser - Профиль: %1")
                      .arg(profile.name));
        m_statusLabel->setText(QString("Переключено на профиль: %1").arg(profile.name));
        m_statusTimer->start();
    });
    
    connect(m_profileManager, &ProfileManager::profileCreated,
            this, [this](const QString &profileId) {
        m_statusLabel->setText(QString("Профиль создан: %1").arg(profileId));
        m_statusTimer->start();
    });
    
    // KSN Client signals
    connect(m_ksnClient, &KsnClient::urlCheckCompleted,
            this, [this](const QUrl &url, const KsnClient::UrlCheckResponse &response) {
        QString statusText;
        switch (response.result) {
            case KsnClient::UrlCheckResult::Safe:
                statusText = QString("✅ Безопасно: %1").arg(url.host());
                break;
            case KsnClient::UrlCheckResult::Suspicious:
                statusText = QString("⚠️ Подозрительно: %1").arg(url.host());
                break;
            case KsnClient::UrlCheckResult::Malicious:
            case KsnClient::UrlCheckResult::Phishing:
                statusText = QString("🛑 Опасно: %1 - %2").arg(url.host()).arg(response.reason);
                break;
            default:
                statusText = QString("❓ Неизвестно: %1").arg(url.host());
        }
        m_statusLabel->setText(statusText);
    });
    
    qDebug() << "All component signals connected";
}