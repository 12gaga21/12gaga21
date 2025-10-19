#ifndef SECURITYSETTINGSDIALOG_H
#define SECURITYSETTINGSDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSlider>
#include <QCheckBox>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QTextEdit>

class SecurityManager;

/**
 * @brief SecuritySettingsDialog - Диалог настроек безопасности
 * 
 * Позволяет пользователю настраивать уровень безопасности браузера,
 * просматривать статистику и управлять настройками KSN.
 */
class SecuritySettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SecuritySettingsDialog(SecurityManager *securityManager, QWidget *parent = nullptr);
    ~SecuritySettingsDialog();

private slots:
    void onSecurityLevelChanged(int level);
    void onApplyClicked();
    void onResetClicked();
    void onClearCacheClicked();
    void updateStatistics();

private:
    void setupUI();
    void loadSettings();
    void saveSettings();
    void updateSecurityLevelDescription(int level);
    
    SecurityManager *m_securityManager;
    
    // UI Components
    QSlider *m_securityLevelSlider;
    QLabel *m_securityLevelLabel;
    QLabel *m_securityDescriptionLabel;
    
    QCheckBox *m_enableKsnCheckbox;
    QCheckBox *m_enableCachingCheckbox;
    QCheckBox *m_blockMaliciousCheckbox;
    QCheckBox *m_blockPhishingCheckbox;
    QCheckBox *m_blockSuspiciousCheckbox;
    
    QLabel *m_totalChecksLabel;
    QLabel *m_blockedUrlsLabel;
    QLabel *m_safeUrlsLabel;
    QLabel *m_suspiciousUrlsLabel;
    QLabel *m_cacheHitsLabel;
    QLabel *m_cacheHitRateLabel;
    
    QSpinBox *m_cacheTimeoutSpinBox;
    QSpinBox *m_requestTimeoutSpinBox;
    
    QPushButton *m_applyButton;
    QPushButton *m_resetButton;
    QPushButton *m_clearCacheButton;
    QPushButton *m_closeButton;
    
    QTimer *m_statsUpdateTimer;
};

#endif // SECURITYSETTINGSDIALOG_H
