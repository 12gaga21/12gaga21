# Руководство по сборке и развертыванию

## 🚀 Быстрый старт

### Требования к системе

- **ОС:** Linux (Ubuntu 20.04+), Windows 10+, macOS 10.15+
- **Qt:** 6.8.3 или выше
- **CMake:** 3.20 или выше
- **Компилятор:** C++20 совместимый
- **Память:** минимум 4 GB RAM
- **Диск:** минимум 2 GB свободного места

### Установка зависимостей

#### Ubuntu/Debian

```bash
# Обновление системы
sudo apt update && sudo apt upgrade -y

# Установка Qt 6.8.3
sudo apt install -y qt6-base-dev qt6-base-dev-tools qt6-base-private-dev
sudo apt install -y qt6-webengine-dev qt6-tools-dev

# Установка других зависимостей
sudo apt install -y cmake build-essential
sudo apt install -y libssl-dev libsqlite3-dev
sudo apt install -y pkg-config

# Установка дополнительных библиотек
sudo apt install -y libxcb-cursor0 libxcb-xinerama0
```

#### Windows

```bash
# Установка через vcpkg
vcpkg install qt6[core,widgets,gui,network,sql,webengine]
vcpkg install openssl sqlite3

# Или через Qt Installer
# Скачать Qt 6.8.3 с официального сайта
# Установить с компонентами: Core, Widgets, Gui, Network, Sql, WebEngine
```

#### macOS

```bash
# Установка через Homebrew
brew install qt@6 cmake openssl sqlite3

# Установка дополнительных зависимостей
brew install pkg-config
```

## 🔧 Сборка проекта

### Автоматическая сборка

```bash
# Клонирование репозитория
git clone <repository-url>
cd kaspersky-shield-browser

# Сборка в debug режиме
./scripts/build.sh debug

# Сборка в release режиме
./scripts/build.sh release
```

### Ручная сборка

```bash
# Создание директории сборки
mkdir build && cd build

# Конфигурация CMake
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      -DCMAKE_VERBOSE_MAKEFILE=ON \
      ..

# Сборка
make -j$(nproc)

# Или с помощью CMake
cmake --build . --parallel
```

### Сборка с тестами

```bash
# Включение тестов
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DBUILD_TESTS=ON \
      ..

# Сборка и запуск тестов
make -j$(nproc)
make test
```

## 📦 Создание пакетов

### Linux (DEB пакет)

```bash
# Установка cpack
sudo apt install -y cpack

# Сборка пакета
cd build
cpack -G DEB

# Результат: KasperskyShieldBrowser-1.0.0-Linux.deb
```

### Windows (NSIS установщик)

```bash
# Установка NSIS
# Скачать с https://nsis.sourceforge.io/

# Сборка установщика
cd build
cpack -G NSIS

# Результат: KasperskyShieldBrowser-1.0.0-win64.exe
```

### macOS (DMG пакет)

```bash
# Сборка DMG
cd build
cpack -G DragNDrop

# Результат: KasperskyShieldBrowser-1.0.0-Darwin.dmg
```

## 🚀 Развертывание

### Локальное развертывание

```bash
# Установка в систему
sudo make install

# Или установка в пользовательскую директорию
cmake -DCMAKE_INSTALL_PREFIX=$HOME/.local ..
make install
```

### Docker развертывание

```dockerfile
# Dockerfile
FROM ubuntu:22.04

# Установка зависимостей
RUN apt-get update && apt-get install -y \
    qt6-base-dev \
    qt6-webengine-dev \
    libssl3 \
    libsqlite3-0 \
    && rm -rf /var/lib/apt/lists/*

# Копирование приложения
COPY build/KasperskyShieldBrowser /usr/local/bin/

# Запуск
CMD ["KasperskyShieldBrowser"]
```

```bash
# Сборка Docker образа
docker build -t kaspersky-shield-browser .

# Запуск контейнера
docker run -it --rm kaspersky-shield-browser
```

## 🔧 Конфигурация

### Переменные окружения

```bash
# Путь к базе данных
export KSB_DATABASE_PATH="/path/to/database.db"

# Уровень логирования
export KSB_LOG_LEVEL="INFO"  # DEBUG, INFO, WARNING, ERROR

# Путь к конфигурации
export KSB_CONFIG_PATH="/path/to/config.ini"

# Режим разработки
export KSB_DEV_MODE="true"
```

### Файл конфигурации

```ini
# config.ini
[database]
path=/home/user/.local/share/KasperskyShieldBrowser/database.db
backup_enabled=true
backup_interval=3600

[security]
ksn_api_url=https://api.ksn.kaspersky.com
ksn_api_key=your_api_key_here
cache_enabled=true
cache_size=1000

[parental_control]
default_time_limit=60
bedtime_start=22:00
bedtime_end=07:00
content_filtering=true

[ui]
theme=dark
language=en_US
show_developer_tools=false
```

## 🧪 Тестирование

### Запуск тестов

```bash
# Все тесты
cd build
make test

# Конкретный тест
./tests/test_security_manager
./tests/test_parental_control
./tests/test_database_manager
./tests/test_profile_manager
```

### Покрытие кода

```bash
# Установка gcov
sudo apt install -y gcov

# Сборка с покрытием
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DENABLE_COVERAGE=ON \
      ..
make -j$(nproc)

# Запуск тестов
make test

# Генерация отчета
gcov src/*.cpp
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_html
```

### Статический анализ

```bash
# Установка clang-tidy
sudo apt install -y clang-tidy

# Анализ кода
clang-tidy src/*.cpp -- -Iinclude -I/usr/include/qt6

# Или через CMake
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
clang-tidy -p . src/*.cpp
```

## 🐛 Отладка

### Debug сборка

```bash
# Сборка в debug режиме
./scripts/build.sh debug

# Запуск с отладчиком
gdb ./build_debug/KasperskyShieldBrowser

# Или с valgrind
valgrind --leak-check=full ./build_debug/KasperskyShieldBrowser
```

### Логирование

```cpp
// Включение детального логирования
QLoggingCategory::setFilterRules("*.debug=true");

// Или через переменную окружения
export QT_LOGGING_RULES="*.debug=true"
```

### Профилирование

```bash
# Установка perf
sudo apt install -y linux-tools-common

# Профилирование
perf record ./build_debug/KasperskyShieldBrowser
perf report
```

## 📊 Производительность

### Оптимизация сборки

```bash
# Release сборка с оптимизациями
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS="-O3 -march=native" \
      ..

# Сборка с LTO
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON \
      ..
```

### Мониторинг производительности

```bash
# Мониторинг использования памяти
htop

# Мониторинг сети
nethogs

# Мониторинг диска
iotop
```

## 🔒 Безопасность

### Проверка безопасности

```bash
# Сканирование зависимостей
sudo apt install -y safety
safety check

# Проверка уязвимостей
sudo apt install -y bandit
bandit -r src/
```

### Подписание кода

```bash
# Генерация ключа
gpg --gen-key

# Подписание исполняемого файла
gpg --armor --detach-sig KasperskyShieldBrowser

# Проверка подписи
gpg --verify KasperskyShieldBrowser.asc
```

## 📋 Чек-лист развертывания

### Перед развертыванием

- [ ] Все тесты проходят
- [ ] Код проверен статическим анализатором
- [ ] Документация обновлена
- [ ] Версия увеличена
- [ ] Changelog обновлен
- [ ] Безопасность проверена

### После развертывания

- [ ] Приложение запускается
- [ ] Все функции работают
- [ ] Производительность приемлема
- [ ] Логи не содержат ошибок
- [ ] Пользователи могут использовать

## 🆘 Устранение неполадок

### Частые проблемы

#### Ошибка: "Qt platform plugin could not be loaded"
```bash
# Решение: Установка недостающих библиотек
sudo apt install -y libxcb-cursor0 libxcb-xinerama0
```

#### Ошибка: "OpenSSL not found"
```bash
# Решение: Установка OpenSSL
sudo apt install -y libssl-dev
```

#### Ошибка: "SQLite not found"
```bash
# Решение: Установка SQLite
sudo apt install -y libsqlite3-dev
```

#### Ошибка: "CMake version too old"
```bash
# Решение: Обновление CMake
sudo apt install -y cmake
# Или установка из snap
sudo snap install cmake --classic
```

### Логи и диагностика

```bash
# Просмотр логов системы
journalctl -u kaspersky-shield-browser

# Проверка зависимостей
ldd ./build_debug/KasperskyShieldBrowser

# Проверка версий
./build_debug/KasperskyShieldBrowser --version
```

---

**Версия документации:** 1.0  
**Дата обновления:** 19 октября 2024  
**Статус:** Актуальная