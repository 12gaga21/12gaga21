#!/bin/bash

# Скрипт для запуска тестов функциональности браузера
# Тестирует: навигацию, поиск, вкладки, адресную строку, интеграцию

set -e

echo "🧪 =============================================="
echo "🧪 Kaspersky Home Shield Browser - Browser Tests"
echo "🧪 =============================================="
echo ""

# Цвета для вывода
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Счетчики тестов
TESTS_PASSED=0
TESTS_FAILED=0
TOTAL_TESTS=0

# Директория сборки
BUILD_DIR="../build_debug"
TESTS_DIR="$BUILD_DIR/tests"

# Функция для запуска теста
run_test() {
    local test_name="$1"
    local test_executable="$2"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${BLUE}🔍 Тест $TOTAL_TESTS: $test_name${NC}"
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    
    if [ -f "$test_executable" ]; then
        echo "Запуск: $test_executable"
        echo ""
        
        if $test_executable; then
            echo -e "${GREEN}✅ ПРОЙДЕН${NC}"
            TESTS_PASSED=$((TESTS_PASSED + 1))
        else
            echo -e "${RED}❌ ПРОВАЛЕН${NC}"
            TESTS_FAILED=$((TESTS_FAILED + 1))
        fi
    else
        echo -e "${YELLOW}⚠️  ПРОПУЩЕН (исполняемый файл не найден)${NC}"
        echo "Ожидаемый путь: $test_executable"
        TESTS_FAILED=$((TESTS_FAILED + 1))
    fi
    
    echo ""
}

# Проверка наличия директории сборки
if [ ! -d "$BUILD_DIR" ]; then
    echo -e "${RED}❌ Директория сборки не найдена: $BUILD_DIR${NC}"
    echo "Сначала выполните сборку проекта: ./scripts/build.sh"
    exit 1
fi

echo -e "${YELLOW}📂 Директория сборки: $BUILD_DIR${NC}"
echo ""

# ==================== ТЕСТЫ АДРЕСНОЙ СТРОКИ ====================
echo -e "${BLUE}═══════════════════════════════════════════${NC}"
echo -e "${BLUE}    1. ТЕСТЫ АДРЕСНОЙ СТРОКИ (AddressBar)${NC}"
echo -e "${BLUE}═══════════════════════════════════════════${NC}"
echo ""
echo "Проверяет:"
echo "  • Ввод URL и поисковых запросов"
echo "  • Валидацию URL"
echo "  • Обработку Enter"
echo "  • Автодополнение https://"
echo "  • Различение URL vs поиск"
echo ""

run_test "AddressBar Tests" "$BUILD_DIR/test_addressbar"

# ==================== ТЕСТЫ ГЛАВНОГО ОКНА ====================
echo -e "${BLUE}═══════════════════════════════════════════${NC}"
echo -e "${BLUE}    2. ТЕСТЫ ГЛАВНОГО ОКНА (MainWindow)${NC}"
echo -e "${BLUE}═══════════════════════════════════════════${NC}"
echo ""
echo "Проверяет:"
echo "  • Создание и инициализацию окна"
echo "  • Навигацию (вперед/назад/обновить)"
echo "  • Управление вкладками"
echo "  • Загрузку страниц"
echo "  • Кнопки навигации"
echo ""

run_test "MainWindow Tests" "$BUILD_DIR/test_mainwindow"

# ==================== ИНТЕГРАЦИОННЫЕ ТЕСТЫ ====================
echo -e "${BLUE}═══════════════════════════════════════════${NC}"
echo -e "${BLUE}    3. ИНТЕГРАЦИОННЫЕ ТЕСТЫ (E2E)${NC}"
echo -e "${BLUE}═══════════════════════════════════════════${NC}"
echo ""
echo "Проверяет:"
echo "  • Взаимодействие Browser + SecurityManager"
echo "  • Взаимодействие Browser + ParentalControl"
echo "  • Взаимодействие Browser + ProfileManager"
echo "  • Полный цикл: поиск -> проверка -> загрузка"
echo "  • Сценарии использования"
echo ""

run_test "Browser Integration Tests" "$BUILD_DIR/test_browser_integration"

# ==================== ИТОГОВЫЙ ОТЧЕТ ====================
echo ""
echo -e "${BLUE}═══════════════════════════════════════════${NC}"
echo -e "${BLUE}           ИТОГОВЫЙ ОТЧЕТ${NC}"
echo -e "${BLUE}═══════════════════════════════════════════${NC}"
echo ""

echo "Всего тестов: $TOTAL_TESTS"
echo -e "${GREEN}✅ Пройдено: $TESTS_PASSED${NC}"

if [ $TESTS_FAILED -gt 0 ]; then
    echo -e "${RED}❌ Провалено: $TESTS_FAILED${NC}"
else
    echo -e "${GREEN}❌ Провалено: $TESTS_FAILED${NC}"
fi

# Вычисляем процент успеха
if [ $TOTAL_TESTS -gt 0 ]; then
    SUCCESS_RATE=$((TESTS_PASSED * 100 / TOTAL_TESTS))
    echo "Успешность: ${SUCCESS_RATE}%"
fi

echo ""

# Детальная статистика
echo -e "${BLUE}─────────────────────────────────────────────${NC}"
echo "Категории тестов:"
echo "  1. AddressBar Tests     - Адресная строка и поиск"
echo "  2. MainWindow Tests     - Основные функции браузера"
echo "  3. Integration Tests    - Интеграция компонентов"
echo -e "${BLUE}─────────────────────────────────────────────${NC}"

echo ""

# Возвращаем код ошибки если есть проваленные тесты
if [ $TESTS_FAILED -gt 0 ]; then
    echo -e "${RED}⚠️  Есть проваленные тесты!${NC}"
    echo "Проверьте вывод выше для деталей."
    exit 1
else
    echo -e "${GREEN}🎉 Все тесты браузера пройдены успешно!${NC}"
    exit 0
fi
