#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QString>
#include <QApplication>

/**
 * @brief ThemeManager - Менеджер тем браузера
 * 
 * Управляет темами оформления (светлая/темная),
 * применяет CSS стили и сохраняет предпочтения пользователя.
 */
class ThemeManager : public QObject
{
    Q_OBJECT

public:
    enum class Theme {
        Light,
        Dark,
        System  // Следовать системной теме
    };
    
    explicit ThemeManager(QObject *parent = nullptr);
    ~ThemeManager();
    
    /**
     * @brief Получить текущую тему
     */
    Theme currentTheme() const { return m_currentTheme; }
    
    /**
     * @brief Применить тему
     * @param theme Тема для применения
     */
    void applyTheme(Theme theme);
    
    /**
     * @brief Переключить между светлой и темной темой
     */
    void toggleTheme();
    
    /**
     * @brief Загрузить сохраненную тему из настроек
     */
    void loadSavedTheme();
    
    /**
     * @brief Сохранить текущую тему в настройки
     */
    void saveTheme();
    
    /**
     * @brief Получить название текущей темы
     */
    QString currentThemeName() const;
    
    /**
     * @brief Проверить, темная ли тема сейчас активна
     */
    bool isDarkTheme() const;

signals:
    void themeChanged(Theme theme);

private:
    Theme m_currentTheme;
    
    QString loadStyleSheet(const QString &filename);
    void applyStyleSheet(const QString &styleSheet);
    QString getThemeFilePath(Theme theme);
};

#endif // THEMEMANAGER_H
