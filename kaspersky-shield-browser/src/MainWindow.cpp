#include "MainWindow.h"
#include "AddressBar.h"
#include "SecurityManager.h"
#include "ParentalControlManager.h"
#include "ProfileManager.h"
#include <QApplication>
#include <QWebEngineView>
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
    , m_progressBar(nullptr)
    , m_statusLabel(nullptr)
    , m_securityManager(nullptr)
    , m_parentalControlManager(nullptr)
    , m_profileManager(nullptr)
    , m_statusTimer(nullptr)
{
    setupUI();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupConnections();
    
    // Initialize managers
    m_securityManager = new SecurityManager(this);
    m_parentalControlManager = new ParentalControlManager(this);
    m_profileManager = new ProfileManager(this);
    
    // Set window properties
    setWindowTitle("Kaspersky Home Shield Browser");
    setMinimumSize(1024, 768);
    resize(1200, 800);
    
    // Create initial tab
    QWebEngineView *webView = new QWebEngineView(this);
    m_tabWidget->addTab(webView, "New Tab");
    m_currentWebView = webView;
    
    // Load default page
    webView->load(QUrl("https://www.kaspersky.com"));
}

MainWindow::~MainWindow()
{
    // Cleanup is handled by Qt's parent-child system
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
    mainLayout->addWidget(m_tabWidget);
    
    // Connect tab signals
    connect(m_tabWidget, &QTabWidget::currentChanged, [this](int index) {
        if (index >= 0) {
            m_currentWebView = qobject_cast<QWebEngineView*>(m_tabWidget->widget(index));
            if (m_currentWebView) {
                updateUrlBar(m_currentWebView->url());
                updateTitle(m_currentWebView->title());
            }
        }
    });
    
    connect(m_tabWidget, &QTabWidget::tabCloseRequested, [this](int index) {
        if (m_tabWidget->count() > 1) {
            m_tabWidget->removeTab(index);
        } else {
            QMessageBox::information(this, "Cannot Close Tab", 
                                   "At least one tab must remain open.");
        }
    });
}

void MainWindow::setupMenuBar()
{
    QMenuBar *menuBar = this->menuBar();
    
    // File menu
    QMenu *fileMenu = menuBar->addMenu("&File");
    fileMenu->addAction("&New Tab", this, [this]() {
        QWebEngineView *webView = new QWebEngineView(this);
        m_tabWidget->addTab(webView, "New Tab");
        m_tabWidget->setCurrentWidget(webView);
        m_currentWebView = webView;
        webView->load(QUrl("https://www.kaspersky.com"));
    }, QKeySequence::New);
    
    fileMenu->addSeparator();
    fileMenu->addAction("&Exit", this, &QWidget::close, QKeySequence::Quit);
    
    // View menu
    QMenu *viewMenu = menuBar->addMenu("&View");
    viewMenu->addAction("&Security Status", this, &MainWindow::showSecurityStatus);
    viewMenu->addAction("&Parental Control", this, &MainWindow::showParentalControl);
    viewMenu->addAction("&Profile Manager", this, &MainWindow::showProfileManager);
    
    // Help menu
    QMenu *helpMenu = menuBar->addMenu("&Help");
    helpMenu->addAction("&About", this, [this]() {
        QMessageBox::about(this, "About Kaspersky Home Shield Browser",
                          "Kaspersky Home Shield Browser v1.0.0\n"
                          "A secure browser with integrated Kaspersky protection.");
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
}

void MainWindow::navigateToUrl()
{
    if (!m_currentWebView) return;
    
    QString url = m_addressBar->text().trimmed();
    if (url.isEmpty()) return;
    
    // Add protocol if missing
    if (!url.startsWith("http://") && !url.startsWith("https://") && !url.startsWith("file://")) {
        if (url.contains(".") && !url.contains(" ")) {
            url = "https://" + url;
        } else {
            url = "https://www.google.com/search?q=" + url;
        }
    }
    
    m_currentWebView->load(QUrl(url));
}

void MainWindow::goBack()
{
    if (m_currentWebView) {
        m_currentWebView->back();
    }
}

void MainWindow::goForward()
{
    if (m_currentWebView) {
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
            m_tabWidget->setTabText(currentIndex, title);
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
    QMessageBox::information(this, "Security Status", 
                           "Security features are active.\n"
                           "Real-time protection is enabled.");
}

void MainWindow::showParentalControl()
{
    QMessageBox::information(this, "Parental Control", 
                           "Parental control settings will be available here.");
}

void MainWindow::showProfileManager()
{
    QMessageBox::information(this, "Profile Manager", 
                           "Profile management will be available here.");
}