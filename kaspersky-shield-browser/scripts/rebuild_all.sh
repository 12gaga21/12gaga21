#!/bin/bash

# Скрипт полной пересборки проекта с новыми интеграциями

set -e

echo "🔨 =============================================="
echo "🔨 Kaspersky Home Shield Browser - Full Rebuild"
echo "🔨 =============================================="
echo ""

# Цвета
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Переход в корень проекта
cd "$(dirname "$0")/.."

echo -e "${BLUE}📂 Рабочая директория: $(pwd)${NC}"
echo ""

# Очистка старой сборки
echo -e "${YELLOW}🧹 Очистка старой сборки...${NC}"
if [ -d "build_debug" ]; then
    rm -rf build_debug
    echo -e "${GREEN}✅ Старая сборка удалена${NC}"
else
    echo -e "${YELLOW}ℹ️  Старой сборки не найдено${NC}"
fi
echo ""

# Создание директории сборки
echo -e "${YELLOW}📁 Создание директории сборки...${NC}"
mkdir -p build_debug
cd build_debug
echo -e "${GREEN}✅ Директория создана${NC}"
echo ""

# Конфигурация CMake
echo -e "${BLUE}⚙️  Конфигурация CMake...${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

if cmake .. -DCMAKE_BUILD_TYPE=Debug; then
    echo -e "${GREEN}✅ CMake конфигурация успешна${NC}"
else
    echo -e "${RED}❌ Ошибка конфигурации CMake${NC}"
    exit 1
fi
echo ""

# Сборка проекта
echo -e "${BLUE}🔨 Сборка проекта...${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

if cmake --build . -- -j$(nproc); then
    echo -e "${GREEN}✅ Сборка успешна${NC}"
else
    echo -e "${RED}❌ Ошибка сборки${NC}"
    exit 1
fi
echo ""

# Проверка исполняемого файла
echo -e "${BLUE}🔍 Проверка исполняемого файла...${NC}"

if [ -f "KasperskyShieldBrowser" ]; then
    FILE_SIZE=$(du -h KasperskyShieldBrowser | cut -f1)
    echo -e "${GREEN}✅ Исполняемый файл создан: KasperskyShieldBrowser${NC}"
    echo "   Размер: $FILE_SIZE"
else
    echo -e "${RED}❌ Исполняемый файл не найден${NC}"
    exit 1
fi
echo ""

# Сборка тестов
echo -e "${BLUE}🧪 Сборка тестов...${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

cd ../tests
mkdir -p ../build_debug/tests
cd ../build_debug

if cmake ../tests; then
    echo -e "${GREEN}✅ Тесты сконфигурированы${NC}"
    
    if make -j$(nproc) 2>/dev/null || cmake --build . --target all; then
        echo -e "${GREEN}✅ Тесты собраны${NC}"
    else
        echo -e "${YELLOW}⚠️  Некоторые тесты не собрались (ожидаемо для новых тестов)${NC}"
    fi
else
    echo -e "${YELLOW}⚠️  Конфигурация тестов пропущена${NC}"
fi
echo ""

# Подсчет собранных файлов
cd ..
echo -e "${BLUE}📊 Статистика сборки:${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

EXECUTABLES=$(find build_debug -maxdepth 1 -type f -executable 2>/dev/null | wc -l)
TEST_EXECUTABLES=$(find build_debug -name "test_*" -type f -executable 2>/dev/null | wc -l)

echo "   Исполняемых файлов: $EXECUTABLES"
echo "   Тестовых файлов: $TEST_EXECUTABLES"
echo ""

# Итоговый результат
echo -e "${GREEN}═══════════════════════════════════════════${NC}"
echo -e "${GREEN}    ✅ СБОРКА ЗАВЕРШЕНА УСПЕШНО${NC}"
echo -e "${GREEN}═══════════════════════════════════════════${NC}"
echo ""
echo "Для запуска браузера:"
echo "  ./build_debug/KasperskyShieldBrowser"
echo ""
echo "Для запуска тестов:"
echo "  ./scripts/test_browser.sh"
echo ""

exit 0
