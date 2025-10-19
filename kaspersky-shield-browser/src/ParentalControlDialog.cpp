#include "ParentalControlDialog.h"
#include "ParentalControlManager.h"
#include "ProfileManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QHeaderView>
#include <QDateTime>

ParentalControlDialog::ParentalControlDialog(ParentalControlManager *parentalControl,
                                           ProfileManager *profileManager,
                                           QWidget *parent)
    : QDialog(parent)
    , m_parentalControl(parentalControl)
    , m_profileManager(profileManager)
{
    setWindowTitle("Родительский контроль");
    setMinimumSize(700, 800);
    
    setupUI();
    loadProfiles();
}

ParentalControlDialog::~ParentalControlDialog()
{
}

void ParentalControlDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // ==================== Profile Selection ====================
    QGroupBox *profileGroup = new QGroupBox("Выбор профиля", this);
    QHBoxLayout *profileLayout = new QHBoxLayout(profileGroup);
    
    profileLayout->addWidget(new QLabel("Профиль:", this));
    m_profileCombo = new QComboBox(this);
    profileLayout->addWidget(m_profileCombo);
    profileLayout->addStretch();
    
    mainLayout->addWidget(profileGroup);
    
    // ==================== Content Filtering ====================
    QGroupBox *filterGroup = new QGroupBox("Фильтрация контента", this);
    QVBoxLayout *filterLayout = new QVBoxLayout(filterGroup);
    
    filterLayout->addWidget(new QLabel("Блокировать следующие категории:", this));
    
    m_adultContentCheckbox = new QCheckBox("🔞 Контент для взрослых", this);
    m_gamblingCheckbox = new QCheckBox("🎰 Азартные игры", this);
    m_violenceCheckbox = new QCheckBox("💥 Насилие", this);
    m_drugsCheckbox = new QCheckBox("💊 Наркотики", this);
    m_weaponsCheckbox = new QCheckBox("🔫 Оружие", this);
    m_terrorismCheckbox = new QCheckBox("⚠️ Терроризм", this);
    m_socialMediaCheckbox = new QCheckBox("📱 Социальные сети", this);
    m_shoppingCheckbox = new QCheckBox("🛒 Интернет-магазины", this);
    
    filterLayout->addWidget(m_adultContentCheckbox);
    filterLayout->addWidget(m_gamblingCheckbox);
    filterLayout->addWidget(m_violenceCheckbox);
    filterLayout->addWidget(m_drugsCheckbox);
    filterLayout->addWidget(m_weaponsCheckbox);
    filterLayout->addWidget(m_terrorismCheckbox);
    filterLayout->addWidget(m_socialMediaCheckbox);
    filterLayout->addWidget(m_shoppingCheckbox);
    
    mainLayout->addWidget(filterGroup);
    
    // ==================== Time Limits ====================
    QGroupBox *timeGroup = new QGroupBox("Временные ограничения", this);
    QVBoxLayout *timeLayout = new QVBoxLayout(timeGroup);
    
    // Daily limit
    QHBoxLayout *dailyLimitLayout = new QHBoxLayout();
    dailyLimitLayout->addWidget(new QLabel("Дневной лимит (минут):", this));
    m_dailyLimitSpinBox = new QSpinBox(this);
    m_dailyLimitSpinBox->setMinimum(0);
    m_dailyLimitSpinBox->setMaximum(1440);
    m_dailyLimitSpinBox->setValue(0);
    m_dailyLimitSpinBox->setSpecialValueText("Без ограничений");
    dailyLimitLayout->addWidget(m_dailyLimitSpinBox);
    dailyLimitLayout->addStretch();
    timeLayout->addLayout(dailyLimitLayout);
    
    // Bed time
    m_enableBedTimeCheckbox = new QCheckBox("Включить режим сна", this);
    timeLayout->addWidget(m_enableBedTimeCheckbox);
    
    QHBoxLayout *bedTimeLayout = new QHBoxLayout();
    bedTimeLayout->addWidget(new QLabel("Время сна:", this));
    m_bedTimeStartEdit = new QTimeEdit(this);
    m_bedTimeStartEdit->setDisplayFormat("HH:mm");
    m_bedTimeStartEdit->setTime(QTime(22, 0));
    bedTimeLayout->addWidget(m_bedTimeStartEdit);
    
    bedTimeLayout->addWidget(new QLabel(" - ", this));
    m_bedTimeEndEdit = new QTimeEdit(this);
    m_bedTimeEndEdit->setDisplayFormat("HH:mm");
    m_bedTimeEndEdit->setTime(QTime(7, 0));
    bedTimeLayout->addWidget(m_bedTimeEndEdit);
    bedTimeLayout->addStretch();
    timeLayout->addLayout(bedTimeLayout);
    
    mainLayout->addWidget(timeGroup);
    
    // ==================== Statistics ====================
    QGroupBox *statsGroup = new QGroupBox("Статистика", this);
    QVBoxLayout *statsLayout = new QVBoxLayout(statsGroup);
    
    m_blockedTodayLabel = new QLabel("Заблокировано сегодня: 0 попыток", this);
    m_timeUsedLabel = new QLabel("Использовано времени: 0 минут", this);
    m_timeRemainingLabel = new QLabel("Осталось времени: Без ограничений", this);
    
    statsLayout->addWidget(m_blockedTodayLabel);
    statsLayout->addWidget(m_timeUsedLabel);
    statsLayout->addWidget(m_timeRemainingLabel);
    
    mainLayout->addWidget(statsGroup);
    
    // ==================== Activity History ====================
    QGroupBox *historyGroup = new QGroupBox("История активности", this);
    QVBoxLayout *historyLayout = new QVBoxLayout(historyGroup);
    
    m_historyTable = new QTableWidget(this);
    m_historyTable->setColumnCount(4);
    m_historyTable->setHorizontalHeaderLabels({"Время", "URL", "Категория", "Статус"});
    m_historyTable->horizontalHeader()->setStretchLastSection(true);
    m_historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_historyTable->setMaximumHeight(150);
    historyLayout->addWidget(m_historyTable);
    
    QHBoxLayout *historyButtonLayout = new QHBoxLayout();
    m_viewHistoryButton = new QPushButton("📖 Показать всю историю", this);
    m_clearHistoryButton = new QPushButton("🗑️ Очистить историю", this);
    historyButtonLayout->addWidget(m_viewHistoryButton);
    historyButtonLayout->addWidget(m_clearHistoryButton);
    historyButtonLayout->addStretch();
    historyLayout->addLayout(historyButtonLayout);
    
    mainLayout->addWidget(historyGroup);
    
    // ==================== Action Buttons ====================
    QHBoxLayout *actionLayout = new QHBoxLayout();
    actionLayout->addStretch();
    
    m_applyButton = new QPushButton("💾 Применить", this);
    m_applyButton->setDefault(true);
    m_resetButton = new QPushButton("🔄 Сбросить", this);
    m_closeButton = new QPushButton("❌ Закрыть", this);
    
    actionLayout->addWidget(m_applyButton);
    actionLayout->addWidget(m_resetButton);
    actionLayout->addWidget(m_closeButton);
    
    mainLayout->addLayout(actionLayout);
    
    // Connect signals
    connect(m_profileCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ParentalControlDialog::onProfileSelected);
    connect(m_applyButton, &QPushButton::clicked, this, &ParentalControlDialog::onApplyClicked);
    connect(m_resetButton, &QPushButton::clicked, this, &ParentalControlDialog::onResetClicked);
    connect(m_viewHistoryButton, &QPushButton::clicked, this, &ParentalControlDialog::onViewHistoryClicked);
    connect(m_clearHistoryButton, &QPushButton::clicked, this, &ParentalControlDialog::onClearHistoryClicked);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
    
    // Connect category checkboxes
    connect(m_adultContentCheckbox, &QCheckBox::toggled, this, &ParentalControlDialog::onCategoryToggled);
    connect(m_gamblingCheckbox, &QCheckBox::toggled, this, &ParentalControlDialog::onCategoryToggled);
    connect(m_violenceCheckbox, &QCheckBox::toggled, this, &ParentalControlDialog::onCategoryToggled);
}

void ParentalControlDialog::loadProfiles()
{
    if (!m_profileManager) return;
    
    m_profileCombo->clear();
    
    QList<UserProfile> profiles = m_profileManager->getAllProfiles();
    QString currentProfileId = m_profileManager->getCurrentProfileId();
    
    int currentIndex = 0;
    for (int i = 0; i < profiles.size(); ++i) {
        const UserProfile &profile = profiles[i];
        
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
        
        m_profileCombo->addItem(icon + " " + profile.name, profile.id);
        
        if (profile.id == currentProfileId) {
            currentIndex = i;
        }
    }
    
    m_profileCombo->setCurrentIndex(currentIndex);
    loadSettings();
}

void ParentalControlDialog::onProfileSelected(int index)
{
    if (index < 0) return;
    
    m_currentProfileId = m_profileCombo->itemData(index).toString();
    loadSettings();
}

void ParentalControlDialog::loadSettings()
{
    if (m_currentProfileId.isEmpty() || !m_profileManager) return;
    
    UserProfile profile = m_profileManager->getProfile(m_currentProfileId);
    
    // Load time limit
    m_dailyLimitSpinBox->setValue(profile.timeLimitMinutes);
    
    // Update statistics
    m_timeUsedLabel->setText(QString("Использовано времени: %1 минут")
                            .arg(profile.currentUsageMinutes));
    
    int remaining = profile.timeLimitMinutes - profile.currentUsageMinutes;
    if (profile.timeLimitMinutes == 0) {
        m_timeRemainingLabel->setText("Осталось времени: Без ограничений");
    } else {
        m_timeRemainingLabel->setText(QString("Осталось времени: %1 минут")
                                     .arg(qMax(0, remaining)));
    }
    
    // Update category checkboxes based on profile type
    updateCategoryCheckboxes();
    
    // Load activity history (recent 5 entries)
    if (m_parentalControl) {
        QList<ActivityRecord> history = m_parentalControl->getActivityHistory(m_currentProfileId);
        
        m_historyTable->setRowCount(qMin(5, history.size()));
        
        for (int i = 0; i < qMin(5, history.size()); ++i) {
            const ActivityRecord &record = history[i];
            
            m_historyTable->setItem(i, 0, new QTableWidgetItem(
                record.timestamp.toString("hh:mm:ss")));
            m_historyTable->setItem(i, 1, new QTableWidgetItem(
                record.url.toString()));
            m_historyTable->setItem(i, 2, new QTableWidgetItem(
                record.category));
            m_historyTable->setItem(i, 3, new QTableWidgetItem(
                record.wasBlocked ? "🛑 Заблокировано" : "✅ Разрешено"));
        }
    }
}

void ParentalControlDialog::saveSettings()
{
    if (m_currentProfileId.isEmpty() || !m_parentalControl || !m_profileManager) return;
    
    // Get current profile
    UserProfile profile = m_profileManager->getProfile(m_currentProfileId);
    
    // Update time limit
    profile.timeLimitMinutes = m_dailyLimitSpinBox->value();
    m_profileManager->updateProfile(profile);
    
    // Update blocked categories
    QStringList blockedCategories;
    if (m_adultContentCheckbox->isChecked()) blockedCategories << "Adult";
    if (m_gamblingCheckbox->isChecked()) blockedCategories << "Gambling";
    if (m_violenceCheckbox->isChecked()) blockedCategories << "Violence";
    if (m_drugsCheckbox->isChecked()) blockedCategories << "Drugs";
    if (m_weaponsCheckbox->isChecked()) blockedCategories << "Weapons";
    if (m_terrorismCheckbox->isChecked()) blockedCategories << "Terrorism";
    if (m_socialMediaCheckbox->isChecked()) blockedCategories << "Social Media";
    if (m_shoppingCheckbox->isChecked()) blockedCategories << "Shopping";
    
    m_parentalControl->setBlockedCategories(m_currentProfileId, blockedCategories);
    
    QMessageBox::information(this, "Успех", "Настройки родительского контроля сохранены.");
}

void ParentalControlDialog::updateCategoryCheckboxes()
{
    if (m_currentProfileId.isEmpty() || !m_profileManager) return;
    
    UserProfile profile = m_profileManager->getProfile(m_currentProfileId);
    
    // Set default blocked categories based on profile type
    switch (profile.type) {
        case ProfileType::Parent:
            // No restrictions for parents
            m_adultContentCheckbox->setChecked(false);
            m_gamblingCheckbox->setChecked(false);
            m_violenceCheckbox->setChecked(false);
            m_drugsCheckbox->setChecked(false);
            m_weaponsCheckbox->setChecked(false);
            m_terrorismCheckbox->setChecked(false);
            m_socialMediaCheckbox->setChecked(false);
            m_shoppingCheckbox->setChecked(false);
            break;
            
        case ProfileType::Child:
            // Maximum restrictions for children
            m_adultContentCheckbox->setChecked(true);
            m_gamblingCheckbox->setChecked(true);
            m_violenceCheckbox->setChecked(true);
            m_drugsCheckbox->setChecked(true);
            m_weaponsCheckbox->setChecked(true);
            m_terrorismCheckbox->setChecked(true);
            m_socialMediaCheckbox->setChecked(false);
            m_shoppingCheckbox->setChecked(false);
            break;
            
        case ProfileType::Teen:
            // Moderate restrictions for teens
            m_adultContentCheckbox->setChecked(true);
            m_gamblingCheckbox->setChecked(true);
            m_violenceCheckbox->setChecked(false);
            m_drugsCheckbox->setChecked(true);
            m_weaponsCheckbox->setChecked(true);
            m_terrorismCheckbox->setChecked(true);
            m_socialMediaCheckbox->setChecked(false);
            m_shoppingCheckbox->setChecked(false);
            break;
    }
}

void ParentalControlDialog::onCategoryToggled()
{
    // Enable/disable apply button when categories change
    m_applyButton->setEnabled(true);
}

void ParentalControlDialog::onApplyClicked()
{
    saveSettings();
}

void ParentalControlDialog::onResetClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Сброс настроек",
        "Вы уверены, что хотите сбросить настройки родительского контроля для этого профиля?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        updateCategoryCheckboxes();
        m_dailyLimitSpinBox->setValue(0);
        QMessageBox::information(this, "Сброс", "Настройки сброшены к значениям по умолчанию.");
    }
}

void ParentalControlDialog::onViewHistoryClicked()
{
    QMessageBox::information(this, "История активности", 
                           "Полная история активности будет доступна в следующей версии.");
}

void ParentalControlDialog::onClearHistoryClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Очистка истории",
        "Вы уверены, что хотите очистить всю историю активности для этого профиля?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        if (m_parentalControl) {
            m_parentalControl->clearActivityHistory(m_currentProfileId);
        }
        m_historyTable->setRowCount(0);
        QMessageBox::information(this, "Очищено", "История активности очищена.");
    }
}
