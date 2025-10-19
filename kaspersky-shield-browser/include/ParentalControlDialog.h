#ifndef PARENTALCONTROLDIALOG_H
#define PARENTALCONTROLDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QTimeEdit>
#include <QTableWidget>

class ParentalControlManager;
class ProfileManager;

/**
 * @brief ParentalControlDialog - Диалог настройки родительского контроля
 * 
 * Позволяет настраивать фильтрацию контента, временные ограничения,
 * просматривать историю активности и управлять блокировками.
 */
class ParentalControlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ParentalControlDialog(ParentalControlManager *parentalControl,
                                  ProfileManager *profileManager,
                                  QWidget *parent = nullptr);
    ~ParentalControlDialog();

private slots:
    void onProfileSelected(int index);
    void onCategoryToggled();
    void onApplyClicked();
    void onResetClicked();
    void onViewHistoryClicked();
    void onClearHistoryClicked();

private:
    void setupUI();
    void loadProfiles();
    void loadSettings();
    void saveSettings();
    void updateCategoryCheckboxes();
    
    ParentalControlManager *m_parentalControl;
    ProfileManager *m_profileManager;
    
    // UI Components
    QComboBox *m_profileCombo;
    
    // Content Filtering
    QCheckBox *m_adultContentCheckbox;
    QCheckBox *m_gamblingCheckbox;
    QCheckBox *m_violenceCheckbox;
    QCheckBox *m_drugsCheckbox;
    QCheckBox *m_weaponsCheckbox;
    QCheckBox *m_terrorismCheckbox;
    QCheckBox *m_socialMediaCheckbox;
    QCheckBox *m_shoppingCheckbox;
    
    // Time Limits
    QSpinBox *m_dailyLimitSpinBox;
    QTimeEdit *m_bedTimeStartEdit;
    QTimeEdit *m_bedTimeEndEdit;
    QCheckBox *m_enableBedTimeCheckbox;
    
    // Statistics
    QLabel *m_blockedTodayLabel;
    QLabel *m_timeUsedLabel;
    QLabel *m_timeRemainingLabel;
    
    // Activity History
    QTableWidget *m_historyTable;
    
    // Buttons
    QPushButton *m_applyButton;
    QPushButton *m_resetButton;
    QPushButton *m_viewHistoryButton;
    QPushButton *m_clearHistoryButton;
    QPushButton *m_closeButton;
    
    QString m_currentProfileId;
};

#endif // PARENTALCONTROLDIALOG_H
