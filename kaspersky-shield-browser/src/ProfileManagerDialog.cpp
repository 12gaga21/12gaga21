#include "ProfileManagerDialog.h"
#include "ProfileManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QDateTime>

ProfileManagerDialog::ProfileManagerDialog(ProfileManager *profileManager, QWidget *parent)
    : QDialog(parent)
    , m_profileManager(profileManager)
    , m_editMode(false)
{
    setWindowTitle("Управление профилями");
    setMinimumSize(800, 600);
    
    setupUI();
    loadProfiles();
}

ProfileManagerDialog::~ProfileManagerDialog()
{
}

void ProfileManagerDialog::setupUI()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    
    // ==================== Left Panel - Profile List ====================
    QVBoxLayout *leftLayout = new QVBoxLayout();
    
    QLabel *listLabel = new QLabel("Профили:", this);
    listLabel->setStyleSheet("font-weight: bold; font-size: 11pt;");
    leftLayout->addWidget(listLabel);
    
    m_profileList = new QListWidget(this);
    m_profileList->setMinimumWidth(250);
    leftLayout->addWidget(m_profileList);
    
    // Buttons
    QHBoxLayout *buttonLayout1 = new QHBoxLayout();
    m_newProfileButton = new QPushButton("➕ Новый", this);
    m_editProfileButton = new QPushButton("✏️ Изменить", this);
    buttonLayout1->addWidget(m_newProfileButton);
    buttonLayout1->addWidget(m_editProfileButton);
    leftLayout->addLayout(buttonLayout1);
    
    QHBoxLayout *buttonLayout2 = new QHBoxLayout();
    m_deleteProfileButton = new QPushButton("🗑️ Удалить", this);
    m_switchProfileButton = new QPushButton("🔄 Переключить", this);
    buttonLayout2->addWidget(m_deleteProfileButton);
    buttonLayout2->addWidget(m_switchProfileButton);
    leftLayout->addLayout(buttonLayout2);
    
    mainLayout->addLayout(leftLayout);
    
    // ==================== Right Panel - Profile Details ====================
    QVBoxLayout *rightLayout = new QVBoxLayout();
    
    QGroupBox *detailsGroup = new QGroupBox("Детали профиля", this);
    QVBoxLayout *detailsLayout = new QVBoxLayout(detailsGroup);
    
    // Profile ID (read-only)
    QHBoxLayout *idLayout = new QHBoxLayout();
    idLayout->addWidget(new QLabel("ID профиля:", this));
    m_profileIdEdit = new QLineEdit(this);
    m_profileIdEdit->setReadOnly(true);
    m_profileIdEdit->setPlaceholderText("Автоматически");
    idLayout->addWidget(m_profileIdEdit);
    detailsLayout->addLayout(idLayout);
    
    // Profile Name
    QHBoxLayout *nameLayout = new QHBoxLayout();
    nameLayout->addWidget(new QLabel("Имя профиля:", this));
    m_profileNameEdit = new QLineEdit(this);
    m_profileNameEdit->setPlaceholderText("Введите имя профиля");
    nameLayout->addWidget(m_profileNameEdit);
    detailsLayout->addLayout(nameLayout);
    
    // Profile Type
    QHBoxLayout *typeLayout = new QHBoxLayout();
    typeLayout->addWidget(new QLabel("Тип профиля:", this));
    m_profileTypeCombo = new QComboBox(this);
    m_profileTypeCombo->addItem("👨‍👩‍👧 Родитель", static_cast<int>(ProfileType::Parent));
    m_profileTypeCombo->addItem("👦 Ребенок", static_cast<int>(ProfileType::Child));
    m_profileTypeCombo->addItem("👨‍🎓 Подросток", static_cast<int>(ProfileType::Teen));
    typeLayout->addWidget(m_profileTypeCombo);
    typeLayout->addStretch();
    detailsLayout->addLayout(typeLayout);
    
    // Time Limit
    QHBoxLayout *timeLimitLayout = new QHBoxLayout();
    timeLimitLayout->addWidget(new QLabel("Дневной лимит (минут):", this));
    m_timeLimitSpinBox = new QSpinBox(this);
    m_timeLimitSpinBox->setMinimum(0);
    m_timeLimitSpinBox->setMaximum(1440); // 24 hours
    m_timeLimitSpinBox->setValue(0);
    m_timeLimitSpinBox->setSpecialValueText("Без ограничений");
    timeLimitLayout->addWidget(m_timeLimitSpinBox);
    timeLimitLayout->addStretch();
    detailsLayout->addLayout(timeLimitLayout);
    
    detailsLayout->addSpacing(20);
    
    // Statistics
    m_currentUsageLabel = new QLabel("Использовано сегодня: 0 минут", this);
    m_lastUsedLabel = new QLabel("Последнее использование: Никогда", this);
    m_createdLabel = new QLabel("Создан: -", this);
    
    detailsLayout->addWidget(m_currentUsageLabel);
    detailsLayout->addWidget(m_lastUsedLabel);
    detailsLayout->addWidget(m_createdLabel);
    
    detailsLayout->addStretch();
    rightLayout->addWidget(detailsGroup);
    
    // Save/Cancel buttons
    QHBoxLayout *actionLayout = new QHBoxLayout();
    actionLayout->addStretch();
    
    QPushButton *saveButton = new QPushButton("💾 Сохранить", this);
    saveButton->setDefault(true);
    QPushButton *cancelButton = new QPushButton("❌ Отмена", this);
    
    actionLayout->addWidget(saveButton);
    actionLayout->addWidget(cancelButton);
    rightLayout->addLayout(actionLayout);
    
    mainLayout->addLayout(rightLayout, 1);
    
    // Connect signals
    connect(m_profileList, &QListWidget::itemClicked, this, &ProfileManagerDialog::onProfileSelected);
    connect(m_newProfileButton, &QPushButton::clicked, this, &ProfileManagerDialog::onNewProfileClicked);
    connect(m_editProfileButton, &QPushButton::clicked, this, &ProfileManagerDialog::onEditProfileClicked);
    connect(m_deleteProfileButton, &QPushButton::clicked, this, &ProfileManagerDialog::onDeleteProfileClicked);
    connect(m_switchProfileButton, &QPushButton::clicked, this, &ProfileManagerDialog::onSwitchProfileClicked);
    connect(saveButton, &QPushButton::clicked, this, &ProfileManagerDialog::onSaveClicked);
    connect(cancelButton, &QPushButton::clicked, this, &ProfileManagerDialog::onCancelClicked);
    
    // Initial state
    m_editProfileButton->setEnabled(false);
    m_deleteProfileButton->setEnabled(false);
    m_switchProfileButton->setEnabled(false);
}

void ProfileManagerDialog::loadProfiles()
{
    m_profileList->clear();
    
    if (!m_profileManager) return;
    
    QList<UserProfile> profiles = m_profileManager->getAllProfiles();
    QString currentProfileId = m_profileManager->getCurrentProfileId();
    
    for (const UserProfile &profile : profiles) {
        QString displayText = profile.name;
        
        // Add icon based on type
        QString icon;
        switch (profile.type) {
            case ProfileType::Parent:
                icon = "👨‍👩‍👧";
                break;
            case ProfileType::Child:
                icon = "👦";
                break;
            case ProfileType::Teen:
                icon = "👨‍🎓";
                break;
        }
        
        displayText = icon + " " + displayText;
        
        // Mark current profile
        if (profile.id == currentProfileId) {
            displayText += " ⭐";
        }
        
        QListWidgetItem *item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, profile.id);
        m_profileList->addItem(item);
    }
}

void ProfileManagerDialog::onProfileSelected(QListWidgetItem *item)
{
    if (!item) return;
    
    QString profileId = item->data(Qt::UserRole).toString();
    m_currentProfileId = profileId;
    
    updateProfileDetails(profileId);
    
    m_editProfileButton->setEnabled(true);
    m_deleteProfileButton->setEnabled(profileId != "parent"); // Can't delete parent
    m_switchProfileButton->setEnabled(true);
}

void ProfileManagerDialog::onNewProfileClicked()
{
    m_editMode = true;
    m_currentProfileId.clear();
    clearProfileDetails();
    
    m_profileNameEdit->setEnabled(true);
    m_profileTypeCombo->setEnabled(true);
    m_timeLimitSpinBox->setEnabled(true);
    m_profileNameEdit->setFocus();
}

void ProfileManagerDialog::onEditProfileClicked()
{
    if (m_currentProfileId.isEmpty()) return;
    
    m_editMode = true;
    m_profileNameEdit->setEnabled(true);
    m_profileTypeCombo->setEnabled(true);
    m_timeLimitSpinBox->setEnabled(true);
}

void ProfileManagerDialog::onDeleteProfileClicked()
{
    if (m_currentProfileId.isEmpty() || m_currentProfileId == "parent") return;
    
    UserProfile profile = m_profileManager->getProfile(m_currentProfileId);
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Удаление профиля",
        QString("Вы уверены, что хотите удалить профиль '%1'?").arg(profile.name),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        if (m_profileManager->deleteProfile(m_currentProfileId)) {
            QMessageBox::information(this, "Успех", "Профиль успешно удален.");
            loadProfiles();
            clearProfileDetails();
            m_currentProfileId.clear();
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось удалить профиль.");
        }
    }
}

void ProfileManagerDialog::onSwitchProfileClicked()
{
    if (m_currentProfileId.isEmpty()) return;
    
    m_profileManager->switchToProfile(m_currentProfileId);
    
    QMessageBox::information(this, "Профиль переключен",
                           QString("Активный профиль: %1")
                           .arg(m_profileManager->getCurrentProfile().name));
    
    loadProfiles();
}

void ProfileManagerDialog::onSaveClicked()
{
    if (!validateProfile()) return;
    
    UserProfile profile;
    
    if (m_currentProfileId.isEmpty()) {
        // New profile
        profile.id = "profile_" + QString::number(QDateTime::currentMSecsSinceEpoch());
        profile.lastResetDate = QDateTime::currentDateTime();
    } else {
        // Edit existing profile
        profile = m_profileManager->getProfile(m_currentProfileId);
    }
    
    profile.name = m_profileNameEdit->text();
    profile.type = static_cast<ProfileType>(m_profileTypeCombo->currentData().toInt());
    profile.timeLimitMinutes = m_timeLimitSpinBox->value();
    profile.isActive = true;
    
    bool success = false;
    if (m_currentProfileId.isEmpty()) {
        success = m_profileManager->createProfile(profile);
    } else {
        success = m_profileManager->updateProfile(profile);
    }
    
    if (success) {
        QMessageBox::information(this, "Успех", 
                               m_currentProfileId.isEmpty() ? "Профиль создан." : "Профиль обновлен.");
        loadProfiles();
        m_editMode = false;
        m_currentProfileId = profile.id;
        updateProfileDetails(profile.id);
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить профиль.");
    }
}

void ProfileManagerDialog::onCancelClicked()
{
    if (m_editMode) {
        m_editMode = false;
        if (m_currentProfileId.isEmpty()) {
            clearProfileDetails();
        } else {
            updateProfileDetails(m_currentProfileId);
        }
    } else {
        accept();
    }
}

void ProfileManagerDialog::updateProfileDetails(const QString &profileId)
{
    if (!m_profileManager) return;
    
    UserProfile profile = m_profileManager->getProfile(profileId);
    
    if (profile.id.isEmpty()) {
        clearProfileDetails();
        return;
    }
    
    m_profileIdEdit->setText(profile.id);
    m_profileNameEdit->setText(profile.name);
    m_profileNameEdit->setEnabled(false);
    
    // Set profile type
    for (int i = 0; i < m_profileTypeCombo->count(); ++i) {
        if (static_cast<ProfileType>(m_profileTypeCombo->itemData(i).toInt()) == profile.type) {
            m_profileTypeCombo->setCurrentIndex(i);
            break;
        }
    }
    m_profileTypeCombo->setEnabled(false);
    
    m_timeLimitSpinBox->setValue(profile.timeLimitMinutes);
    m_timeLimitSpinBox->setEnabled(false);
    
    m_currentUsageLabel->setText(QString("Использовано сегодня: %1 минут")
                                .arg(profile.currentUsageMinutes));
    
    if (profile.lastResetDate.isValid()) {
        m_lastUsedLabel->setText(QString("Последнее использование: %1")
                                .arg(profile.lastResetDate.toString("dd.MM.yyyy hh:mm")));
    }
    
    m_createdLabel->setText(QString("Создан: %1")
                           .arg(profile.lastResetDate.toString("dd.MM.yyyy")));
}

void ProfileManagerDialog::clearProfileDetails()
{
    m_profileIdEdit->clear();
    m_profileNameEdit->clear();
    m_profileTypeCombo->setCurrentIndex(0);
    m_timeLimitSpinBox->setValue(0);
    m_currentUsageLabel->setText("Использовано сегодня: 0 минут");
    m_lastUsedLabel->setText("Последнее использование: Никогда");
    m_createdLabel->setText("Создан: -");
}

bool ProfileManagerDialog::validateProfile()
{
    if (m_profileNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка валидации", "Введите имя профиля.");
        return false;
    }
    
    return true;
}
