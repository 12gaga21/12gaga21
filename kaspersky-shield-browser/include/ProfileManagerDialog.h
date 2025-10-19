#ifndef PROFILEMANAGERDIALOG_H
#define PROFILEMANAGERDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QGroupBox>
#include "CommonTypes.h"

class ProfileManager;

/**
 * @brief ProfileManagerDialog - Диалог управления профилями пользователей
 * 
 * Позволяет создавать, редактировать и удалять профили,
 * настраивать их параметры и переключаться между ними.
 */
class ProfileManagerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProfileManagerDialog(ProfileManager *profileManager, QWidget *parent = nullptr);
    ~ProfileManagerDialog();

private slots:
    void onProfileSelected(QListWidgetItem *item);
    void onNewProfileClicked();
    void onEditProfileClicked();
    void onDeleteProfileClicked();
    void onSwitchProfileClicked();
    void onSaveClicked();
    void onCancelClicked();

private:
    void setupUI();
    void loadProfiles();
    void updateProfileDetails(const QString &profileId);
    void clearProfileDetails();
    bool validateProfile();
    
    ProfileManager *m_profileManager;
    
    // UI Components - Profile List
    QListWidget *m_profileList;
    QPushButton *m_newProfileButton;
    QPushButton *m_editProfileButton;
    QPushButton *m_deleteProfileButton;
    QPushButton *m_switchProfileButton;
    
    // UI Components - Profile Details
    QLineEdit *m_profileNameEdit;
    QLineEdit *m_profileIdEdit;
    QComboBox *m_profileTypeCombo;
    QSpinBox *m_timeLimitSpinBox;
    QLabel *m_currentUsageLabel;
    QLabel *m_lastUsedLabel;
    QLabel *m_createdLabel;
    
    // State
    QString m_currentProfileId;
    bool m_editMode;
};

#endif // PROFILEMANAGERDIALOG_H
