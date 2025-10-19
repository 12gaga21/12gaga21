# 🎨 KASPERSKY DESIGN SYSTEM 2025

**Версия:** 2.0.0  
**Дата:** 19 октября 2025  
**Статус:** 📋 COMPLETE DESIGN SPECIFICATION

---

## 📑 СОДЕРЖАНИЕ

1. [Философия дизайна](#философия-дизайна)
2. [Цветовая система](#цветовая-система)
3. [Типографика](#типографика)
4. [Компоненты UI](#компоненты-ui)
5. [Иконография](#иконография)
6. [Анимации](#анимации)
7. [Layout System](#layout-system)
8. [Accessibility](#accessibility)

---

## 💭 ФИЛОСОФИЯ ДИЗАЙНА

### Принципы

```
1️⃣ БЕЗОПАСНОСТЬ ВИЗУАЛИЗИРОВАНА
   • Пользователь всегда видит уровень защиты
   • Цветовая кодировка безопасности
   • Понятные индикаторы

2️⃣ МИНИМАЛИЗМ С ФУНКЦИЕЙ
   • Только необходимые элементы
   • Прогрессивное раскрытие
   • Фокус на контенте

3️⃣ KASPERSKY ИДЕНТИЧНОСТЬ
   • Узнаваемый брендинг
   • Корпоративные цвета (#00A88E)
   • Профессиональный вид

4️⃣ СОВРЕМЕННОСТЬ 2025
   • Темная тема по умолчанию
   • Glassmorphism эффекты
   • Плавные transitions
   • Touch-friendly UI

5️⃣ ДОСТУПНОСТЬ ДЛЯ ВСЕХ
   • WCAG 2.1 AA compliance
   • Большие touch-targets
   • Keyboard navigation
   • Screen reader support
```

---

## 🎨 ЦВЕТОВАЯ СИСТЕМА

### Primary Colors (Основные)

#### Темная тема (Default)

```css
/* Brand Colors */
--kaspersky-primary:    #00A88E;  /* Kaspersky бирюзовый */
--kaspersky-secondary:  #00C9A7;  /* Светлый бирюзовый */
--kaspersky-accent:     #00D4B3;  /* Яркий бирюзовый (hover) */

/* Background */
--bg-primary:           #0F0F0F;  /* Глубокий черный */
--bg-secondary:         #1A1A1A;  /* Темно-серый */
--bg-tertiary:          #242424;  /* Средне-серый */
--bg-elevated:          #2D2D2D;  /* Поднятый (cards, dialogs) */

/* Text */
--text-primary:         #FFFFFF;  /* Белый */
--text-secondary:       #B4B4B4;  /* Светло-серый */
--text-tertiary:        #6B6B6B;  /* Серый */
--text-disabled:        #4D4D4D;  /* Disabled */

/* Borders */
--border-primary:       #333333;  /* Основная граница */
--border-secondary:     #404040;  /* Светлая граница */
--border-focus:         #00A88E;  /* Focus state */

/* Status Colors */
--status-success:       #00D98C;  /* Зеленый */
--status-warning:       #FFB020;  /* Оранжевый */
--status-danger:        #FF4757;  /* Красный */
--status-info:          #4E8FFF;  /* Синий */

/* Security Levels */
--security-safe:        #00D98C;  /* Безопасно */
--security-suspicious:  #FFB020;  /* Подозрительно */
--security-dangerous:   #FF4757;  /* Опасно */
--security-unknown:     #808080;  /* Неизвестно */
```

#### Светлая тема

```css
/* Background */
--bg-primary:           #FFFFFF;  /* Белый */
--bg-secondary:         #F5F5F7;  /* Светло-серый */
--bg-tertiary:          #EBEBED;  /* Серый */
--bg-elevated:          #FFFFFF;  /* Белый (с тенью) */

/* Text */
--text-primary:         #1D1D1F;  /* Почти черный */
--text-secondary:       #6E6E73;  /* Темно-серый */
--text-tertiary:        #86868B;  /* Серый */

/* Borders */
--border-primary:       #D2D2D7;  /* Основная */
--border-secondary:     #E5E5EA;  /* Светлая */
```

### Color Usage Guidelines

```
✅ DO (Правильно):
  • Используйте --kaspersky-primary для акцентов
  • Используйте --status-* для статусов
  • Контраст текста minimum 4.5:1
  • Бренд-цвета для кнопок действий

❌ DON'T (Неправильно):
  • Не используйте чистые цвета (#FF0000)
  • Не миксуйте темную/светлую палитры
  • Не используйте status-цвета для декораций
  • Не игнорируйте контрастность
```

### Security Color Mapping

```typescript
const securityColors = {
  safe: {
    color: '#00D98C',
    icon: '✅',
    meaning: 'Сайт безопасен'
  },
  
  suspicious: {
    color: '#FFB020',
    icon: '⚠️',
    meaning: 'Подозрительный сайт'
  },
  
  dangerous: {
    color: '#FF4757',
    icon: '🛑',
    meaning: 'Опасный сайт'
  },
  
  unknown: {
    color: '#808080',
    icon: '❓',
    meaning: 'Неизвестный статус'
  }
};
```

---

## 🔤 ТИПОГРАФИКА

### Font Stack

```css
/* Системные шрифты для кросс-платформенности */
font-family: -apple-system, BlinkMacSystemFont, 
             "Segoe UI", "Roboto", 
             "Helvetica Neue", Arial, 
             sans-serif;

/* Моноширинный (для кода) */
font-family-mono: "SF Mono", "Consolas", 
                  "Liberation Mono", "Menlo", 
                  "Courier", monospace;

/* Dyslexia-friendly (опция) */
font-family-dyslexia: "OpenDyslexic", sans-serif;
```

### Type Scale

```css
/* Heading Styles */
.heading-1 {
  font-size: 32px;
  font-weight: 700;
  line-height: 1.2;
  letter-spacing: -0.02em;
}

.heading-2 {
  font-size: 24px;
  font-weight: 600;
  line-height: 1.3;
  letter-spacing: -0.01em;
}

.heading-3 {
  font-size: 20px;
  font-weight: 600;
  line-height: 1.4;
}

.heading-4 {
  font-size: 16px;
  font-weight: 600;
  line-height: 1.5;
}

/* Body Text */
.body-large {
  font-size: 16px;
  font-weight: 400;
  line-height: 1.5;
}

.body {
  font-size: 14px;
  font-weight: 400;
  line-height: 1.5;
}

.body-small {
  font-size: 12px;
  font-weight: 400;
  line-height: 1.4;
}

/* UI Elements */
.button-text {
  font-size: 14px;
  font-weight: 500;
  line-height: 1;
  letter-spacing: 0.01em;
}

.caption {
  font-size: 12px;
  font-weight: 400;
  line-height: 1.3;
  color: var(--text-secondary);
}

.overline {
  font-size: 11px;
  font-weight: 600;
  line-height: 1;
  letter-spacing: 0.08em;
  text-transform: uppercase;
}
```

### Font Weights

```
300 - Light      (редко)
400 - Regular    (основной текст)
500 - Medium     (кнопки, labels)
600 - Semibold   (заголовки)
700 - Bold       (H1, важные элементы)
```

---

## 🧩 КОМПОНЕНТЫ UI

### Buttons

#### Primary Button (Акцентная)

```css
.button-primary {
  /* Градиент Kaspersky */
  background: linear-gradient(90deg, 
    #00A88E 0%, 
    #00C9A7 100%
  );
  border: none;
  border-radius: 10px;
  padding: 12px 24px;
  min-height: 44px;
  
  font-size: 14px;
  font-weight: 600;
  color: #FFFFFF;
  
  cursor: pointer;
  transition: all 0.2s ease;
}

.button-primary:hover {
  background: linear-gradient(90deg, 
    #00C9A7 0%, 
    #00D4B3 100%
  );
  transform: translateY(-1px);
  box-shadow: 0 4px 12px rgba(0, 168, 142, 0.3);
}

.button-primary:active {
  transform: scale(0.98);
}

.button-primary:disabled {
  background: #2D2D2D;
  color: #6B6B6B;
  cursor: not-allowed;
}
```

#### Secondary Button

```css
.button-secondary {
  background: #2D2D2D;
  border: 1px solid #404040;
  border-radius: 10px;
  padding: 12px 24px;
  min-height: 44px;
  
  font-size: 14px;
  font-weight: 500;
  color: #FFFFFF;
  
  transition: all 0.2s ease;
}

.button-secondary:hover {
  background: #363636;
  border-color: #00A88E;
  box-shadow: 0 0 0 3px rgba(0, 168, 142, 0.1);
}
```

#### Icon Button

```css
.button-icon {
  background: transparent;
  border: none;
  border-radius: 10px;
  width: 44px;
  height: 44px;
  padding: 10px;
  
  display: flex;
  align-items: center;
  justify-content: center;
  
  cursor: pointer;
  transition: all 0.15s ease;
}

.button-icon:hover {
  background: rgba(255, 255, 255, 0.08);
}

.button-icon:active {
  background: rgba(255, 255, 255, 0.12);
}
```

### Input Fields

```css
.input {
  background: #242424;
  border: 2px solid #333333;
  border-radius: 12px;
  padding: 12px 20px;
  
  font-size: 15px;
  color: #FFFFFF;
  
  transition: all 0.2s ease;
}

.input:hover {
  border-color: #404040;
}

.input:focus {
  outline: none;
  background: #2D2D2D;
  border-color: #00A88E;
  box-shadow: 0 0 0 4px rgba(0, 168, 142, 0.15);
}

.input::placeholder {
  color: #6B6B6B;
}

/* Address Bar (Omnibox) */
.input-omnibox {
  min-height: 44px;
  padding-left: 48px;  /* For search icon */
  padding-right: 48px; /* For actions */
  border-radius: 12px;
  font-size: 16px;
}
```

### Cards

```css
.card {
  background: #1A1A1A;
  border: 1px solid #333333;
  border-radius: 12px;
  padding: 20px;
  
  transition: all 0.2s ease;
}

.card:hover {
  border-color: #404040;
  transform: translateY(-2px);
  box-shadow: 0 8px 24px rgba(0, 0, 0, 0.3);
}

.card-elevated {
  background: #2D2D2D;
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.2);
}
```

### Dialogs

```css
.dialog {
  background: #1A1A1A;
  border: 1px solid #333333;
  border-radius: 16px;
  
  /* Glassmorphism effect */
  backdrop-filter: blur(20px);
  box-shadow: 0 16px 48px rgba(0, 0, 0, 0.5);
  
  max-width: 600px;
  width: 90%;
}

.dialog-header {
  padding: 24px 24px 16px;
  border-bottom: 1px solid #333333;
}

.dialog-content {
  padding: 24px;
}

.dialog-footer {
  padding: 16px 24px;
  border-top: 1px solid #333333;
  display: flex;
  gap: 12px;
  justify-content: flex-end;
}
```

### Tabs

```css
.tabs {
  display: flex;
  gap: 4px;
  border-bottom: 1px solid #333333;
}

.tab {
  background: transparent;
  border: none;
  border-bottom: 2px solid transparent;
  border-radius: 10px 10px 0 0;
  padding: 12px 20px;
  
  min-width: 120px;
  max-width: 240px;
  
  font-size: 14px;
  font-weight: 500;
  color: #B4B4B4;
  
  cursor: pointer;
  transition: all 0.2s ease;
}

.tab:hover {
  background: rgba(255, 255, 255, 0.05);
  color: #FFFFFF;
}

.tab-active {
  background: #1A1A1A;
  border-bottom-color: #00A88E;
  color: #FFFFFF;
}

.tab-close {
  opacity: 0;
  margin-left: 8px;
  transition: opacity 0.2s ease;
}

.tab:hover .tab-close {
  opacity: 1;
}
```

### Progress Bar

```css
.progress {
  background: #242424;
  border-radius: 3px;
  height: 6px;
  overflow: hidden;
}

.progress-bar {
  background: linear-gradient(90deg, 
    #00A88E 0%, 
    #00C9A7 100%
  );
  height: 100%;
  border-radius: 3px;
  
  transition: width 0.3s ease;
}
```

### Checkboxes & Radio

```css
.checkbox {
  width: 20px;
  height: 20px;
  border: 2px solid #404040;
  border-radius: 6px;
  background: #242424;
  
  transition: all 0.15s ease;
}

.checkbox:hover {
  border-color: #00A88E;
}

.checkbox-checked {
  background: #00A88E;
  border-color: #00A88E;
  
  /* Checkmark */
  background-image: url("data:image/svg+xml,...");
}

.radio {
  width: 20px;
  height: 20px;
  border: 2px solid #404040;
  border-radius: 10px;
  background: #242424;
}

.radio-checked {
  background: #00A88E;
  border-color: #00A88E;
  
  /* Inner dot */
  box-shadow: inset 0 0 0 4px #242424;
}
```

### Sliders

```css
.slider {
  width: 100%;
  height: 6px;
  background: #2D2D2D;
  border-radius: 3px;
  position: relative;
}

.slider-track {
  background: linear-gradient(90deg, 
    #00A88E 0%, 
    #00C9A7 100%
  );
  height: 100%;
  border-radius: 3px;
}

.slider-thumb {
  width: 20px;
  height: 20px;
  background: linear-gradient(180deg, 
    #00C9A7 0%, 
    #00A88E 100%
  );
  border: 2px solid #FFFFFF;
  border-radius: 10px;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.3);
  
  cursor: grab;
  transition: all 0.15s ease;
}

.slider-thumb:hover {
  transform: scale(1.1);
}

.slider-thumb:active {
  cursor: grabbing;
  transform: scale(1.05);
}
```

---

## 🎭 ИКОНОГРАФИЯ

### Icon System

```
Стиль: Outline (контурные) + Filled (заливка для активных)
Размеры: 16px, 20px, 24px, 32px
Толщина линии: 2px
Скругление углов: 2px
```

### Core Icons (Emoji-based для кросс-платформенности)

```
Навигация:
  ◀ - Назад
  ▶ - Вперед
  ↻ - Обновить
  ✕ - Стоп/Закрыть
  ➕ - Добавить
  ⭐ - Избранное
  
Безопасность:
  🔒 - Безопасное соединение (HTTPS)
  🔓 - Небезопасное (HTTP)
  🛡️ - Защита Kaspersky
  🔐 - Родительский контроль
  
Статус:
  ✅ - Безопасно
  ⚠️ - Предупреждение
  🛑 - Опасно
  ❓ - Неизвестно
  ℹ️ - Информация
  
Действия:
  👤 - Профиль
  ⚙️ - Настройки
  🌓 - Переключить тему
  📥 - Загрузка
  📤 - Выгрузка
  🔍 - Поиск
```

### Icon Usage

```css
.icon {
  font-size: 20px;
  width: 24px;
  height: 24px;
  display: inline-flex;
  align-items: center;
  justify-content: center;
}

.icon-small {
  font-size: 16px;
  width: 20px;
  height: 20px;
}

.icon-large {
  font-size: 24px;
  width: 32px;
  height: 32px;
}
```

---

## ✨ АНИМАЦИИ

### Transitions

```css
/* Базовый timing function */
--ease-standard: cubic-bezier(0.4, 0.0, 0.2, 1);
--ease-decelerate: cubic-bezier(0.0, 0.0, 0.2, 1);
--ease-accelerate: cubic-bezier(0.4, 0.0, 1, 1);

/* Длительности */
--duration-short: 100ms;
--duration-medium: 200ms;
--duration-long: 300ms;

/* Применение */
.animated-element {
  transition: all var(--duration-medium) var(--ease-standard);
}
```

### Hover Effects

```css
/* Кнопки */
.button:hover {
  transform: translateY(-1px);
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.2);
}

/* Карточки */
.card:hover {
  transform: translateY(-2px);
}

/* Ссылки */
.link:hover {
  color: var(--kaspersky-accent);
}
```

### Loading States

```css
@keyframes spin {
  from { transform: rotate(0deg); }
  to { transform: rotate(360deg); }
}

.loading-spinner {
  animation: spin 1s linear infinite;
}

@keyframes pulse {
  0%, 100% { opacity: 1; }
  50% { opacity: 0.5; }
}

.loading-dots {
  animation: pulse 1.5s ease-in-out infinite;
}
```

### Page Transitions

```css
.page-enter {
  opacity: 0;
  transform: translateX(20px);
}

.page-enter-active {
  opacity: 1;
  transform: translateX(0);
  transition: all 300ms ease-out;
}

.page-exit {
  opacity: 1;
}

.page-exit-active {
  opacity: 0;
  transform: translateX(-20px);
  transition: all 200ms ease-in;
}
```

---

## 📐 LAYOUT SYSTEM

### Spacing Scale

```css
/* Base: 4px */
--space-0: 0;
--space-1: 4px;
--space-2: 8px;
--space-3: 12px;
--space-4: 16px;
--space-5: 20px;
--space-6: 24px;
--space-8: 32px;
--space-10: 40px;
--space-12: 48px;
--space-16: 64px;
```

### Grid System

```css
.container {
  max-width: 1440px;
  margin: 0 auto;
  padding: 0 var(--space-6);
}

.row {
  display: flex;
  gap: var(--space-4);
}

.col {
  flex: 1;
}

.col-2 {
  flex: 0 0 calc(50% - var(--space-2));
}

.col-3 {
  flex: 0 0 calc(33.333% - var(--space-3));
}

.col-4 {
  flex: 0 0 calc(25% - var(--space-3));
}
```

### Z-Index Scale

```css
--z-base: 0;
--z-dropdown: 1000;
--z-sticky: 1100;
--z-fixed: 1200;
--z-modal-backdrop: 1300;
--z-modal: 1400;
--z-popover: 1500;
--z-tooltip: 1600;
--z-toast: 1700;
```

---

## ♿ ACCESSIBILITY

### Contrast Ratios

```
Text (normal):     4.5:1 minimum (WCAG AA)
Text (large 18px): 3:1 minimum
UI Components:     3:1 minimum
```

### Focus States

```css
*:focus {
  outline: none;
  box-shadow: 0 0 0 4px rgba(0, 168, 142, 0.15);
}

.button:focus {
  border-color: var(--kaspersky-primary);
  box-shadow: 0 0 0 4px rgba(0, 168, 142, 0.15);
}
```

### Touch Targets

```
Минимум: 44x44px (iOS HIG / Material Design)
Рекомендуется: 48x48px
Отступ между элементами: 8px минимум
```

### Screen Reader Support

```html
<!-- Semantic HTML -->
<button aria-label="Закрыть" aria-pressed="false">
  ✕
</button>

<!-- Hidden text for screen readers -->
<span class="sr-only">
  Безопасное соединение
</span>

<!-- Live regions -->
<div role="status" aria-live="polite">
  Загрузка завершена
</div>
```

---

## 🎯 ЗАКЛЮЧЕНИЕ

**Kaspersky Design System 2025** обеспечивает:

```
✅ Единообразие UI/UX
✅ Современный минималистичный дизайн
✅ Kaspersky брендинг (#00A88E)
✅ Темная и светлая темы
✅ Accessibility (WCAG 2.1 AA)
✅ Touch-friendly (44x44px)
✅ Плавные анимации
✅ Кросс-платформенность
```

---

**📅 Версия:** 2.0.0  
**📅 Дата:** 19 октября 2025  
**🎨 Kaspersky Home Shield Browser**  
**Дизайн. Минимализм. Безопасность.**
