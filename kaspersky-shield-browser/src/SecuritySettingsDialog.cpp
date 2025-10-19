#include "SecuritySettingsDialog.h"
#include "SecurityManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSlider>
#include <QCheckBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTimer>
#include <QMessageBox>

SecuritySettingsDialog::SecuritySettingsDialog(SecurityManager *securityManager, QWidget *parent)
    : QDialog(parent)
    , m_securityManager(securityManager)
    , m_statsUpdateTimer(new QTimer(this))
{
    setWindowTitle("Настройки безопасности");
    setMinimumSize(600, 700);
    
    setupUI();
    loadSettings();
    
    // Update statistics every 2 seconds
    connect(m_statsUpdateTimer, &QTimer::timeout, this, &SecuritySettingsDialog::updateStatistics);
    m_statsUpdateTimer->start(2000);
    
    // Initial statistics update
    updateStatistics();
}

SecuritySettingsDialog::~SecuritySettingsDialog()
{
    m_statsUpdateTimer->stop();
}

void SecuritySettingsDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // ==================== Security Level Section ====================
    QGroupBox *levelGroup = new QGroupBox("Уровень безопасности", this);
    QVBoxLayout *levelLayout = new QVBoxLayout(levelGroup);
    
    m_securityLevelLabel = new QLabel("Уровень: 5 (Средний)", this);
    m_securityLevelLabel->setStyleSheet("font-weight: bold; font-size: 12pt;");
    levelLayout->addWidget(m_securityLevelLabel);
    
    m_securityLevelSlider = new QSlider(Qt::Horizontal, this);
    m_securityLevelSlider->setMinimum(1);
    m_securityLevelSlider->setMaximum(8);
    m_securityLevelSlider->setValue(5);
    m_securityLevelSlider->setTickPosition(QSlider::TicksBelow);
    m_securityLevelSlider->setTickInterval(1);
    levelLayout->addWidget(m_securityLevelSlider);
    
    // Labels for slider
    QHBoxLayout *levelLabelsLayout = new QHBoxLayout();
    levelLabelsLayout->addWidget(new QLabel("Минимальный", this));
    levelLabelsLayout->addStretch();
    levelLabelsLayout->addWidget(new QLabel("Максимальный", this));
    levelLayout->addLayout(levelLabelsLayout);
    
    m_securityDescriptionLabel = new QLabel(this);
    m_securityDescriptionLabel->setWordWrap(true);
    m_securityDescriptionLabel->setStyleSheet("background-color: #f0f0f0; padding: 10px; border-radius: 5px;");
    levelLayout->addWidget(m_securityDescriptionLabel);
    
    mainLayout->addWidget(levelGroup);
    
    // ==================== Protection Settings Section ====================
    QGroupBox *protectionGroup = new QGroupBox("Настройки защиты", this);
    QVBoxLayout *protectionLayout = new QVBoxLayout(protectionGroup);
    
    m_enableKsnCheckbox = new QCheckBox("Использовать Kaspersky Security Network (KSN)", this);
    m_enableKsnCheckbox->setChecked(true);
    protectionLayout->addWidget(m_enableKsnCheckbox);
    
    m_enableCachingCheckbox = new QCheckBox("Включить кэширование результатов проверки", this);
    m_enableCachingCheckbox->setChecked(true);
    protectionLayout->addWidget(m_enableCachingCheckbox);
    
    m_blockMaliciousCheckbox = new QCheckBox("Блокировать вредоносные сайты", this);
    m_blockMaliciousCheckbox->setChecked(true);
    protectionLayout->addWidget(m_blockMaliciousCheckbox);
    
    m_blockPhishingCheckbox = new QCheckBox("Блокировать фишинговые сайты", this);
    m_blockPhishingCheckbox->setChecked(true);
    protectionLayout->addWidget(m_blockPhishingCheckbox);
    
    m_blockSuspiciousCheckbox = new QCheckBox("Предупреждать о подозрительных сайтах", this);
    m_blockSuspiciousCheckbox->setChecked(true);
    protectionLayout->addWidget(m_blockSuspiciousCheckbox);
    
    mainLayout->addWidget(protectionGroup);
    
    // ==================== Performance Settings Section ====================
    QGroupBox *performanceGroup = new QGroupBox("Настройки производительности", this);
    QVBoxLayout *performanceLayout = new QVBoxLayout(performanceGroup);
    
    QHBoxLayout *cacheTimeoutLayout = new QHBoxLayout();
    cacheTimeoutLayout->addWidget(new QLabel("Время жизни кэша (часы):", this));
    m_cacheTimeoutSpinBox = new QSpinBox(this);
    m_cacheTimeoutSpinBox->setMinimum(1);
    m_cacheTimeoutSpinBox->setMaximum(168); // 7 days
    m_cacheTimeoutSpinBox->setValue(24);
    cacheTimeoutLayout->addWidget(m_cacheTimeoutSpinBox);
    cacheTimeoutLayout->addStretch();
    performanceLayout->addLayout(cacheTimeoutLayout);
    
    QHBoxLayout *requestTimeoutLayout = new QHBoxLayout();
    requestTimeoutLayout->addWidget(new QLabel("Таймаут запроса (секунды):", this));
    m_requestTimeoutSpinBox = new QSpinBox(this);
    m_requestTimeoutSpinBox->setMinimum(1);
    m_requestTimeoutSpinBox->setMaximum(30);
    m_requestTimeoutSpinBox->setValue(5);
    requestTimeoutLayout->addWidget(m_requestTimeoutSpinBox);
    requestTimeoutLayout->addStretch();
    performanceLayout->addLayout(requestTimeoutLayout);
    
    m_clearCacheButton = new QPushButton("Очистить кэш", this);
    performanceLayout->addWidget(m_clearCacheButton);
    
    mainLayout->addWidget(performanceGroup);
    
    // ==================== Statistics Section ====================
    QGroupBox *statsGroup = new QGroupBox("Статистика", this);
    QVBoxLayout *statsLayout = new QVBoxLayout(statsGroup);
    
    m_totalChecksLabel = new QLabel("Всего проверок: 0", this);
    m_blockedUrlsLabel = new QLabel("Заблокировано URL: 0", this);
    m_safeUrlsLabel = new QLabel("Безопасных URL: 0", this);
    m_suspiciousUrlsLabel = new QLabel("Подозрительных URL: 0", this);
    m_cacheHitsLabel = new QLabel("Попаданий в кэш: 0", this);
    m_cacheHitRateLabel = new QLabel("Коэффициент попаданий: 0%", this);
    
    statsLayout->addWidget(m_totalChecksLabel);
    statsLayout->addWidget(m_blockedUrlsLabel);
    statsLayout->addWidget(m_safeUrlsLabel);
    statsLayout->addWidget(m_suspiciousUrlsLabel);
    statsLayout->addWidget(m_cacheHitsLabel);
    statsLayout->addWidget(m_cacheHitRateLabel);
    
    mainLayout->addWidget(statsGroup);
    
    // ==================== Buttons Section ====================
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch();
    
    m_applyButton = new QPushButton("Применить", this);
    m_applyButton->setDefault(true);
    buttonsLayout->addWidget(m_applyButton);
    
    m_resetButton = new QPushButton("Сбросить", this);
    buttonsLayout->addWidget(m_resetButton);
    
    m_closeButton = new QPushButton("Закрыть", this);
    buttonsLayout->addWidget(m_closeButton);
    
    mainLayout->addLayout(buttonsLayout);
    
    // Connect signals
    connect(m_securityLevelSlider, &QSlider::valueChanged, 
            this, &SecuritySettingsDialog::onSecurityLevelChanged);
    connect(m_applyButton, &QPushButton::clicked, 
            this, &SecuritySettingsDialog::onApplyClicked);
    connect(m_resetButton, &QPushButton::clicked, 
            this, &SecuritySettingsDialog::onResetClicked);
    connect(m_clearCacheButton, &QPushButton::clicked, 
            this, &SecuritySettingsDialog::onClearCacheClicked);
    connect(m_closeButton, &QPushButton::clicked, 
            this, &QDialog::accept);
    
    // Initial description update
    updateSecurityLevelDescription(5);
}

void SecuritySettingsDialog::loadSettings()
{
    // Load settings from SecurityManager
    // For now, use default values
}

void SecuritySettingsDialog::saveSettings()
{
    // Save settings to SecurityManager
    QMessageBox::information(this, "Настройки сохранены", 
                           "Настройки безопасности успешно применены.");
}

void SecuritySettingsDialog::onSecurityLevelChanged(int level)
{
    m_securityLevelLabel->setText(QString("Уровень: %1").arg(level));
    updateSecurityLevelDescription(level);
}

void SecuritySettingsDialog::onApplyClicked()
{
    saveSettings();
}

void SecuritySettingsDialog::onResetClicked()
{
    m_securityLevelSlider->setValue(5);
    m_enableKsnCheckbox->setChecked(true);
    m_enableCachingCheckbox->setChecked(true);
    m_blockMaliciousCheckbox->setChecked(true);
    m_blockPhishingCheckbox->setChecked(true);
    m_blockSuspiciousCheckbox->setChecked(true);
    m_cacheTimeoutSpinBox->setValue(24);
    m_requestTimeoutSpinBox->setValue(5);
    
    QMessageBox::information(this, "Сброс настроек", 
                           "Настройки сброшены до значений по умолчанию.");
}

void SecuritySettingsDialog::onClearCacheClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Очистить кэш", 
        "Вы уверены, что хотите очистить кэш проверок безопасности?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        // Clear cache logic here
        QMessageBox::information(this, "Кэш очищен", 
                               "Кэш проверок безопасности успешно очищен.");
        updateStatistics();
    }
}

void SecuritySettingsDialog::updateStatistics()
{
    if (!m_securityManager) return;
    
    auto stats = m_securityManager->getStatistics();
    
    m_totalChecksLabel->setText(QString("Всего проверок: %1").arg(stats.totalChecks));
    m_blockedUrlsLabel->setText(QString("Заблокировано URL: %1").arg(stats.blockedUrls));
    m_safeUrlsLabel->setText(QString("Безопасных URL: %1").arg(stats.safeUrls));
    m_suspiciousUrlsLabel->setText(QString("Подозрительных URL: %1").arg(stats.suspiciousUrls));
    m_cacheHitsLabel->setText(QString("Попаданий в кэш: %1").arg(stats.cacheHits));
    
    double hitRate = stats.totalChecks > 0 
        ? (double)stats.cacheHits / stats.totalChecks * 100.0 
        : 0.0;
    m_cacheHitRateLabel->setText(QString("Коэффициент попаданий: %1%").arg(hitRate, 0, 'f', 1));
}

void SecuritySettingsDialog::updateSecurityLevelDescription(int level)
{
    QString description;
    
    switch (level) {
        case 1:
            description = "🟢 <b>Уровень 1 - Минимальная защита</b><br>"
                         "Только базовая проверка. JavaScript полностью включен. "
                         "Рекомендуется для доверенных сайтов и опытных пользователей.";
            break;
        case 2:
            description = "🟢 <b>Уровень 2 - Низкая защита</b><br>"
                         "Базовая защита с предупреждениями. JavaScript включен. "
                         "Минимальные ограничения на веб-функции.";
            break;
        case 3:
            description = "🟡 <b>Уровень 3 - Ниже среднего</b><br>"
                         "Умеренная защита. JavaScript включен с ограничениями на опасные функции. "
                         "Блокировка некоторых плагинов.";
            break;
        case 4:
            description = "🟡 <b>Уровень 4 - Средний минус</b><br>"
                         "Хорошая защита. JavaScript с ограничениями. WebGL отключен. "
                         "Блокировка подозрительных скриптов.";
            break;
        case 5:
            description = "🟠 <b>Уровень 5 - Средний (Рекомендуемый)</b><br>"
                         "Сбалансированная защита для повседневного использования. "
                         "XSS аудит включен, плагины отключены, JavaScript с контролем.";
            break;
        case 6:
            description = "🟠 <b>Уровень 6 - Выше среднего</b><br>"
                         "Усиленная защита. Строгие ограничения JavaScript. "
                         "Блокировка геолокации и буфера обмена. WebRTC ограничен.";
            break;
        case 7:
            description = "🔴 <b>Уровень 7 - Высокая защита</b><br>"
                         "Максимальная защита с минимальными удобствами. "
                         "Многие веб-функции ограничены. Подходит для детских профилей.";
            break;
        case 8:
            description = "🔴 <b>Уровень 8 - Максимальная защита</b><br>"
                         "Параноидальный режим. Почти все опасные функции отключены. "
                         "Может нарушить работу некоторых сайтов.";
            break;
        default:
            description = "Неизвестный уровень безопасности.";
    }
    
    m_securityDescriptionLabel->setText(description);
}
