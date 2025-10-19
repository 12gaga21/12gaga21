#include "ThemeManager.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QSettings>
#include <QApplication>
#include <QStyleFactory>

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
    , m_currentTheme(Theme::Dark)
{
    qDebug() << "ThemeManager created";
}

ThemeManager::~ThemeManager()
{
    saveTheme();
}

void ThemeManager::applyTheme(Theme theme)
{
    m_currentTheme = theme;
    
    QString styleSheetPath = getThemeFilePath(theme);
    QString styleSheet = loadStyleSheet(styleSheetPath);
    
    if (!styleSheet.isEmpty()) {
        applyStyleSheet(styleSheet);
        qDebug() << "Applied theme:" << currentThemeName();
        emit themeChanged(theme);
    } else {
        qWarning() << "Failed to load theme:" << styleSheetPath;
    }
}

void ThemeManager::toggleTheme()
{
    if (m_currentTheme == Theme::Dark) {
        applyTheme(Theme::Light);
    } else {
        applyTheme(Theme::Dark);
    }
}

void ThemeManager::loadSavedTheme()
{
    QSettings settings("Kaspersky", "ShieldBrowser");
    QString themeName = settings.value("theme", "dark").toString();
    
    Theme theme = Theme::Dark;
    if (themeName == "light") {
        theme = Theme::Light;
    } else if (themeName == "dark") {
        theme = Theme::Dark;
    } else if (themeName == "system") {
        theme = Theme::System;
    }
    
    applyTheme(theme);
    qDebug() << "Loaded saved theme:" << themeName;
}

void ThemeManager::saveTheme()
{
    QSettings settings("Kaspersky", "ShieldBrowser");
    
    QString themeName;
    switch (m_currentTheme) {
        case Theme::Light:
            themeName = "light";
            break;
        case Theme::Dark:
            themeName = "dark";
            break;
        case Theme::System:
            themeName = "system";
            break;
    }
    
    settings.setValue("theme", themeName);
    qDebug() << "Saved theme:" << themeName;
}

QString ThemeManager::currentThemeName() const
{
    switch (m_currentTheme) {
        case Theme::Light:
            return "Light";
        case Theme::Dark:
            return "Dark";
        case Theme::System:
            return "System";
    }
    return "Unknown";
}

bool ThemeManager::isDarkTheme() const
{
    return m_currentTheme == Theme::Dark;
}

QString ThemeManager::loadStyleSheet(const QString &filename)
{
    QFile file(filename);
    
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        // Попытка загрузить из resources
        QString resourcePath = ":/" + filename;
        QFile resourceFile(resourcePath);
        
        if (!resourceFile.open(QFile::ReadOnly | QFile::Text)) {
            qWarning() << "Cannot open stylesheet file:" << filename;
            qWarning() << "Also tried:" << resourcePath;
            return QString();
        }
        
        QTextStream in(&resourceFile);
        return in.readAll();
    }
    
    QTextStream in(&file);
    QString styleSheet = in.readAll();
    file.close();
    
    return styleSheet;
}

void ThemeManager::applyStyleSheet(const QString &styleSheet)
{
    if (QApplication::instance()) {
        QApplication::instance()->setStyleSheet(styleSheet);
        
        // Установка стиля Fusion для лучшего отображения
        QApplication::setStyle(QStyleFactory::create("Fusion"));
        
        qDebug() << "StyleSheet applied successfully";
    } else {
        qWarning() << "No QApplication instance to apply stylesheet";
    }
}

QString ThemeManager::getThemeFilePath(Theme theme)
{
    QString basePath = "resources/styles/";
    
    switch (theme) {
        case Theme::Light:
            return basePath + "modern_light.qss";
        case Theme::Dark:
            return basePath + "modern_dark.qss";
        case Theme::System:
            // TODO: Определить системную тему
            return basePath + "modern_dark.qss";
    }
    
    return basePath + "modern_dark.qss";
}
