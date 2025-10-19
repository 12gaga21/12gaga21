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
#include <QWebEngineDownloadItem>
#include <QWebEngineCertificateError>
#include <QNetworkReply>
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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

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
    void onDownloadRequested(QWebEngineDownloadItem *download);
    void onCertificateError(const QWebEngineCertificateError &error);
    void onUrlChanged(const QUrl &url);
    void onLoadFinished(bool success);
    void onLoadStarted();
    void onLoadProgress(int progress);

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
    
    // Managers
    SecurityManager *m_securityManager;
    ParentalControlManager *m_parentalControlManager;
    ProfileManager *m_profileManager;
    
    // State
    QTimer *m_statusTimer;
    QWebEngineProfile *m_webProfile;
    bool m_isLoading;
};

#endif // MAINWINDOW_H