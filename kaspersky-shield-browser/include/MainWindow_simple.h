#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QStatusBar>
#include <QMenuBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>

QT_BEGIN_NAMESPACE
class QTextEdit;
class QLineEdit;
class QPushButton;
class QLabel;
class QProgressBar;
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
    void updateUrlBar(const QString &url);
    void updateTitle(const QString &title);
    void updateLoadingProgress(int progress);
    void showSecurityStatus();
    void showParentalControl();
    void showProfileManager();

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupConnections();
    
    // UI Components
    QTabWidget *m_tabWidget;
    QTextEdit *m_currentWebView;
    AddressBar *m_addressBar;
    QPushButton *m_backButton;
    QPushButton *m_forwardButton;
    QPushButton *m_refreshButton;
    QPushButton *m_stopButton;
    QPushButton *m_securityButton;
    QPushButton *m_parentalControlButton;
    QPushButton *m_profileButton;
    QProgressBar *m_progressBar;
    QLabel *m_statusLabel;
    
    // Managers
    SecurityManager *m_securityManager;
    ParentalControlManager *m_parentalControlManager;
    ProfileManager *m_profileManager;
    
    // State
    QTimer *m_statusTimer;
};

#endif // MAINWINDOW_H