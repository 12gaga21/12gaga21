#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QStatusBar>
#include <QMenuBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QTimer>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWebEngineHistory>
#include <QUrl>

QT_BEGIN_NAMESPACE
class QWebEngineView;
class QLineEdit;
class QPushButton;
class QLabel;
class QProgressBar;
class QTabWidget;
QT_END_NAMESPACE

class SecurityManager;
class ParentalControlManager;
class ProfileManager;
class AddressBar;
class KsnClient;
class DatabaseManager;
class ThemeManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    // Component access
    KsnClient* ksnClient() const { return m_ksnClient; }
    DatabaseManager* databaseManager() const { return m_databaseManager; }
    SecurityManager* securityManager() const { return m_securityManager; }
    ParentalControlManager* parentalControlManager() const { return m_parentalControlManager; }
    ProfileManager* profileManager() const { return m_profileManager; }

private slots:
    void navigateToUrl();
    void goBack();
    void goForward();
    void refreshPage();
    void stopLoading();
    void updateUrlBar(const QUrl &url);
    void updateTitle(const QString &title);
    void updateLoadingProgress(int progress);
    void showSecurityStatus();
    void showParentalControl();
    void showProfileManager();
    void onTabCloseRequested(int index);
    void onCurrentTabChanged(int index);
    void onNewTabRequested();
    void onDownloadRequested();
    void onCertificateError();
    void onUrlChanged(const QUrl &url);
    void onLoadFinished(bool success);
    void onLoadStarted();
    void onLoadProgress(int progress);
    void toggleTheme();

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupConnections();
    void setupWebEngine();
    QWebEngineView* createNewTab(const QString &url = QString());
    void closeTab(int index);
    void updateNavigationButtons();
    void checkUrlSecurity(const QUrl &url);
    
    // UI Components
    QTabWidget *m_tabWidget;
    QWebEngineView *m_currentWebView;
    AddressBar *m_addressBar;
    QPushButton *m_backButton;
    QPushButton *m_forwardButton;
    QPushButton *m_refreshButton;
    QPushButton *m_stopButton;
    QPushButton *m_securityButton;
    QPushButton *m_parentalControlButton;
    QPushButton *m_profileButton;
    QPushButton *m_newTabButton;
    QProgressBar *m_progressBar;
    QLabel *m_statusLabel;
    
    // Core components
    KsnClient *m_ksnClient;
    DatabaseManager *m_databaseManager;
    ThemeManager *m_themeManager;
    
    // Managers
    SecurityManager *m_securityManager;
    ParentalControlManager *m_parentalControlManager;
    ProfileManager *m_profileManager;
    
    // State
    QTimer *m_statusTimer;
    QWebEngineProfile *m_webProfile;
    bool m_isLoading;
    
    // Initialization
    void initializeComponents();
    void connectComponents();
};

#endif // MAINWINDOW_H