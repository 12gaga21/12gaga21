# 📘 ПОЛНАЯ ТЕХНИЧЕСКАЯ СПЕЦИФИКАЦИЯ - Kaspersky Shield Browser v1.0.0

**Версия документа:** 1.0.0  
**Дата:** 19 октября 2025  
**Статус:** Production Release  
**Тип:** Полная техническая спецификация для реализации

---

## 📋 СОДЕРЖАНИЕ

1. [Обзор проекта](#обзор-проекта)
2. [Архитектура системы](#архитектура-системы)
3. [Технологический стек](#технологический-стек)
4. [Языки программирования](#языки-программирования)
5. [Фреймворки и библиотеки](#фреймворки-и-библиотеки)
6. [База данных](#база-данных)
7. [API спецификация](#api-спецификация)
8. [Безопасность](#безопасность)
9. [Структура проекта](#структура-проекта)
10. [Сборка и развертывание](#сборка-и-развертывание)
11. [Тестирование](#тестирование)
12. [Производительность](#производительность)
13. [Локализация](#локализация)
14. [Зависимости](#зависимости)

---

## 1. ОБЗОР ПРОЕКТА

### 1.1 Назначение

**Kaspersky Shield Browser** — кроссплатформенный безопасный веб-браузер для семей и организаций с встроенной защитой Kaspersky Security Network.

### 1.2 Целевая платформа

```yaml
Desktop:
  - Windows: 10, 11 (64-bit)
  - macOS: 11 Big Sur+
  - Linux: Ubuntu 20.04+, Fedora 35+, Debian 11+

Mobile (Roadmap):
  - Android: 10+ (API level 29+)
  - iOS: 14+
```

### 1.3 Ключевые требования

```
Функциональные:
  ✅ Веб-навигация (вкладки, история, закладки)
  ✅ Безопасность (KSN integration, malware detection)
  ✅ Родительский контроль (content filtering, time limits)
  ✅ Синхронизация (E2E encrypted cloud sync)
  ✅ Приватность (tracker blocking, fingerprint protection)

Нефункциональные:
  ✅ Производительность: <2s page load, <500MB RAM/tab
  ✅ Надежность: 99.9% uptime, crash recovery
  ✅ Безопасность: GDPR, COPPA compliant
  ✅ Масштабируемость: 1M+ concurrent users
  ✅ Совместимость: Chrome extension API
```

---

## 2. АРХИТЕКТУРА СИСТЕМЫ

### 2.1 Общая архитектура

```
┌─────────────────────────────────────────────────────────────────────┐
│                        CLIENT APPLICATION                           │
│                                                                     │
│  ┌──────────────────────────────────────────────────────────────┐ │
│  │                    Presentation Layer                         │ │
│  │                                                                │ │
│  │  ┌────────────┐  ┌────────────┐  ┌────────────┐             │ │
│  │  │ React UI   │  │ Qt Widgets │  │  WebView   │             │ │
│  │  │ (Settings) │  │  (Native)  │  │ (Chromium) │             │ │
│  │  └─────┬──────┘  └─────┬──────┘  └─────┬──────┘             │ │
│  └────────┼────────────────┼────────────────┼────────────────────┘ │
│           │                │                │                      │
│  ┌────────▼────────────────▼────────────────▼────────────────────┐ │
│  │                    Business Logic Layer                       │ │
│  │                                                                │ │
│  │  ┌───────────┐  ┌───────────┐  ┌───────────┐  ┌──────────┐  │ │
│  │  │ Security  │  │ Parental  │  │ Profile   │  │   Sync   │  │ │
│  │  │ Manager   │  │ Control   │  │ Manager   │  │ Service  │  │ │
│  │  └─────┬─────┘  └─────┬─────┘  └─────┬─────┘  └────┬─────┘  │ │
│  └────────┼────────────────┼────────────────┼──────────┼────────┘ │
│           │                │                │          │          │
│  ┌────────▼────────────────▼────────────────▼──────────▼────────┐ │
│  │                     Data Access Layer                         │ │
│  │                                                                │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐       │ │
│  │  │   SQLite     │  │     Cache    │  │     IPC      │       │ │
│  │  │   (Local)    │  │   (Memory)   │  │   (Sockets)  │       │ │
│  │  └──────────────┘  └──────────────┘  └──────────────┘       │ │
│  └────────────────────────────────────────────────────────────────┘ │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
                                 │
                                 │ HTTPS / WSS
                                 ▼
┌─────────────────────────────────────────────────────────────────────┐
│                         BACKEND SERVICES                            │
│                                                                     │
│  ┌──────────────────────────────────────────────────────────────┐ │
│  │                         API Gateway                           │ │
│  │           (Express.js / Fastify with TypeScript)              │ │
│  └──────────────┬───────────────────────────┬────────────────────┘ │
│                 │                           │                      │
│  ┌──────────────▼────────┐   ┌──────────────▼────────┐           │
│  │    Auth Service       │   │    Sync Service       │           │
│  │  (JWT, OAuth 2.0)     │   │  (E2E Encryption)     │           │
│  └───────────────────────┘   └───────────────────────┘           │
│                                                                     │
│  ┌──────────────────────────────────────────────────────────────┐ │
│  │                     Database Layer                            │ │
│  │                                                                │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐       │ │
│  │  │ PostgreSQL   │  │    Redis     │  │   RabbitMQ   │       │ │
│  │  │  (Primary)   │  │   (Cache)    │  │   (Queue)    │       │ │
│  │  └──────────────┘  └──────────────┘  └──────────────┘       │ │
│  └────────────────────────────────────────────────────────────────┘ │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
                                 │
                                 │ REST API
                                 ▼
┌─────────────────────────────────────────────────────────────────────┐
│                      EXTERNAL SERVICES                              │
│                                                                     │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐             │
│  │ Kaspersky    │  │    Cloud     │  │  Analytics   │             │
│  │  KSN API     │  │   Storage    │  │  (Optional)  │             │
│  └──────────────┘  └──────────────┘  └──────────────┘             │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### 2.2 Компонентная архитектура

```cpp
// C++ компоненты (Native Layer)

MainWindow
  ├── BrowserTab (QtWebEngineView)
  │   ├── WebEnginePage (security policies)
  │   └── WebEngineProfile (session management)
  │
  ├── SecurityManager
  │   ├── KsnClient (threat detection)
  │   ├── CertificateValidator
  │   └── SecurityEventLogger
  │
  ├── ParentalControlManager
  │   ├── ContentFilter (URL/category blocking)
  │   ├── TimeManager (usage limits)
  │   └── ActivityMonitor (logging)
  │
  ├── ProfileManager
  │   ├── UserProfile (settings, preferences)
  │   ├── ProfileStorage (SQLite)
  │   └── ProfileSync (cloud sync)
  │
  ├── SyncService
  │   ├── CloudClient (REST API)
  │   ├── EncryptionManager (E2E)
  │   └── ConflictResolver
  │
  └── DatabaseManager
      ├── BookmarkRepository
      ├── HistoryRepository
      └── SettingsRepository
```

```typescript
// TypeScript компоненты (Frontend Layer)

App
  ├── Browser
  │   ├── TabBar
  │   │   ├── Tab (title, favicon, close button)
  │   │   └── NewTabButton
  │   │
  │   ├── NavigationBar
  │   │   ├── BackButton
  │   │   ├── ForwardButton
  │   │   ├── RefreshButton
  │   │   ├── OmniBox (search + URL)
  │   │   └── SecurityIndicator
  │   │
  │   └── WebView (iframe to QtWebEngine)
  │
  ├── Settings
  │   ├── GeneralSettings
  │   ├── SecuritySettings
  │   ├── ParentalControlSettings
  │   ├── ProfileSettings
  │   └── SyncSettings
  │
  └── Stores (Zustand)
      ├── browserStore (tabs, history)
      ├── securityStore (threats, blocks)
      ├── userStore (auth, profile)
      └── syncStore (sync status)
```

### 2.3 Многопроцессная архитектура (Chromium)

```
Browser Process (main)
  │
  ├─► GPU Process (graphics acceleration)
  │
  ├─► Network Process (HTTP/HTTPS, caching)
  │
  ├─► Renderer Process 1 (tab 1 - isolated)
  │   └─► Content (HTML, CSS, JS)
  │
  ├─► Renderer Process 2 (tab 2 - isolated)
  │   └─► Content (HTML, CSS, JS)
  │
  └─► Utility Process (file operations, parsing)
```

**Преимущества:**
- ✅ Изоляция: сбой одной вкладки не влияет на другие
- ✅ Безопасность: sandboxing на уровне процесса
- ✅ Производительность: параллельная обработка

---

## 3. ТЕХНОЛОГИЧЕСКИЙ СТЕК

### 3.1 Полный стек технологий

```yaml
Frontend (UI):
  Languages:
    - TypeScript: 5.3.3
    - HTML5: Latest
    - CSS3: Latest
  
  Frameworks:
    - React: 18.2.0
    - React DOM: 18.2.0
    - React Router: 6.20.0
  
  State Management:
    - Zustand: 4.4.7
  
  Data Fetching:
    - TanStack Query (React Query): 5.14.0
    - Axios: 1.6.2
  
  Styling:
    - Tailwind CSS: 3.3.6
    - PostCSS: 8.4.32
    - Autoprefixer: 10.4.16
  
  Build Tools:
    - Vite: 5.0.6
    - TypeScript Compiler: 5.3.3
    - ESLint: 8.55.0
    - Prettier: 3.1.0

Native Layer (Core):
  Languages:
    - C++: C++20 (GCC 11+, Clang 14+, MSVC 2022+)
  
  UI Framework:
    - Qt: 6.8.3
    - Qt Core: 6.8.3
    - Qt Widgets: 6.8.3
    - Qt WebEngine: 6.8.3 (Chromium 118+)
    - Qt Network: 6.8.3
    - Qt SQL: 6.8.3
  
  Build System:
    - CMake: 3.31.6
    - Ninja: 1.11.1 (optional)
    - Make: 4.3+

Backend (Services):
  Languages:
    - TypeScript: 5.3.3
    - Node.js: 20.10.0 LTS
  
  Framework:
    - Express.js: 4.18.2
    - Fastify: 4.25.1 (alternative)
  
  ORM:
    - Prisma: 5.7.0
    - TypeORM: 0.3.17 (alternative)
  
  Validation:
    - Zod: 3.22.4
    - Joi: 17.11.0 (alternative)

Database:
  Relational:
    - PostgreSQL: 15.5
    - SQLite: 3.45.0
  
  Cache:
    - Redis: 7.2.3
  
  Queue:
    - RabbitMQ: 3.12.10
    - Bull: 4.12.0 (Redis-based)

Security:
  Encryption:
    - OpenSSL: 3.0.12
    - Libsodium: 1.0.19
  
  Hashing:
    - Argon2: 0.31.1
  
  TLS:
    - OpenSSL: 3.0.12 (TLS 1.3)
  
  Authentication:
    - jsonwebtoken: 9.0.2
    - passport: 0.7.0
    - passport-jwt: 4.0.1

Testing:
  Unit Testing:
    - Vitest: 1.0.4 (TypeScript/React)
    - Google Test: 1.14.0 (C++)
    - Qt Test: 6.8.3 (Qt components)
  
  Integration Testing:
    - Playwright: 1.40.1
    - Supertest: 6.3.3
  
  Coverage:
    - Vitest Coverage: v8
    - lcov: 2.0.0

DevOps:
  Containerization:
    - Docker: 24.0.7
    - Docker Compose: 2.23.0
  
  Orchestration:
    - Kubernetes: 1.28.4 (optional)
  
  CI/CD:
    - GitHub Actions: latest
    - GitLab CI: latest (alternative)
  
  Monitoring:
    - Prometheus: 2.48.0
    - Grafana: 10.2.2
  
  Logging:
    - Elasticsearch: 8.11.1
    - Logstash: 8.11.1
    - Kibana: 8.11.1
    - Winston: 3.11.0 (Node.js)

Documentation:
  - Doxygen: 1.9.8 (C++)
  - TypeDoc: 0.25.4 (TypeScript)
  - Swagger/OpenAPI: 3.0.3 (API)
  - MkDocs: 1.5.3 (user docs)
```

---

## 4. ЯЗЫКИ ПРОГРАММИРОВАНИЯ

### 4.1 C++ (Core Application)

**Версия:** C++20  
**Стандарт:** ISO/IEC 14882:2020  
**Компиляторы:**
- GCC: 11.0+
- Clang: 14.0+
- MSVC: 19.30+ (Visual Studio 2022)

**Использование:**
```cpp
// Основное приложение
src/
├── main.cpp                 // Entry point
├── MainWindow.cpp/.h        // Main window
├── managers/                // Business logic
│   ├── SecurityManager.cpp/.h
│   ├── ProfileManager.cpp/.h
│   ├── ParentalControlManager.cpp/.h
│   └── DatabaseManager.cpp/.h
├── services/                // External services
│   ├── KsnClient.cpp/.h
│   └── SyncService.cpp/.h
└── utils/                   // Utilities
    ├── Crypto.cpp/.h
    └── Network.cpp/.h
```

**Ключевые features C++20:**
- Concepts (type constraints)
- Coroutines (async programming)
- Ranges (functional programming)
- Modules (faster compilation)
- std::format (modern string formatting)
- std::span (safe array views)

**Пример кода:**
```cpp
// C++20 with concepts
#include <concepts>
#include <ranges>
#include <format>

template<std::integral T>
T add(T a, T b) {
    return a + b;
}

// Using ranges
std::vector<int> numbers = {1, 2, 3, 4, 5};
auto even = numbers 
    | std::views::filter([](int n) { return n % 2 == 0; })
    | std::views::transform([](int n) { return n * 2; });

// Modern string formatting
std::string message = std::format("User {} logged in at {}", username, time);
```

### 4.2 TypeScript (Frontend + Backend)

**Версия:** 5.3.3  
**Target:** ES2020  
**Module:** ESNext

**Использование:**

**Frontend (React):**
```typescript
// src/components/Browser/TabBar.tsx
import React from 'react';
import { useStore } from '@/stores/browserStore';

interface Tab {
  id: string;
  title: string;
  url: string;
  favicon?: string;
}

export const TabBar: React.FC = () => {
  const { tabs, activeTab, createTab, closeTab } = useStore();
  
  return (
    <div className="tab-bar">
      {tabs.map(tab => (
        <Tab 
          key={tab.id} 
          {...tab} 
          isActive={tab.id === activeTab}
          onClose={() => closeTab(tab.id)}
        />
      ))}
      <button onClick={createTab}>+</button>
    </div>
  );
};
```

**Backend (Node.js):**
```typescript
// server/controllers/authController.ts
import { Request, Response, NextFunction } from 'express';
import { z } from 'zod';
import { AuthService } from '@/services/authService';

const loginSchema = z.object({
  email: z.string().email(),
  password: z.string().min(8),
});

export class AuthController {
  async login(req: Request, res: Response, next: NextFunction) {
    try {
      const { email, password } = loginSchema.parse(req.body);
      const token = await AuthService.authenticate(email, password);
      
      res.json({ token, expiresIn: '24h' });
    } catch (error) {
      next(error);
    }
  }
}
```

### 4.3 JavaScript (Legacy + Utilities)

**Версия:** ES2020  
**Использование:** Build scripts, configuration

```javascript
// scripts/build.js
const { execSync } = require('child_process');
const path = require('path');

function buildProject() {
  console.log('Building Kaspersky Shield Browser...');
  
  // Build frontend
  execSync('npm run build:frontend', { stdio: 'inherit' });
  
  // Build native
  execSync('cmake --build build --config Release', { stdio: 'inherit' });
  
  console.log('Build complete!');
}

buildProject();
```

### 4.4 HTML5

**Стандарт:** HTML5 (WHATWG Living Standard)

```html
<!-- resources/homepage/index.html -->
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="description" content="Kaspersky Shield Browser">
    <title>Kaspersky Shield Browser</title>
</head>
<body>
    <div id="root"></div>
    <script type="module" src="/src/main.tsx"></script>
</body>
</html>
```

### 4.5 CSS3

**Стандарт:** CSS3 + CSS Grid + Flexbox

```css
/* resources/styles/main.css */
:root {
  --primary-color: #00A88E;
  --text-color: #1D1D1F;
}

.container {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
  gap: 1rem;
}
```

### 4.6 SQL (Database Queries)

**Диалекты:**
- PostgreSQL (server)
- SQLite (local)

```sql
-- PostgreSQL
CREATE TABLE users (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    email VARCHAR(255) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    created_at TIMESTAMP DEFAULT NOW()
);

CREATE INDEX idx_users_email ON users(email);
```

```sql
-- SQLite
CREATE TABLE bookmarks (
    id TEXT PRIMARY KEY,
    profile_id TEXT NOT NULL,
    title TEXT NOT NULL,
    url TEXT NOT NULL,
    created_at INTEGER NOT NULL,
    FOREIGN KEY (profile_id) REFERENCES profiles(id)
);
```

### 4.7 Python (Tools & Scripts)

**Версия:** 3.11+  
**Использование:** Build scripts, code generation

```python
#!/usr/bin/env python3
# scripts/generate_icons.py

from PIL import Image
import os

def generate_icon(size):
    """Generate icon of specific size"""
    img = Image.new('RGB', (size, size), color='#00A88E')
    img.save(f'icons/icon_{size}x{size}.png')

sizes = [16, 32, 48, 64, 128, 256, 512]
for size in sizes:
    generate_icon(size)
```

### 4.8 Shell Scripts (Bash)

**Использование:** Build automation, deployment

```bash
#!/bin/bash
# scripts/build.sh

set -e

echo "Building Kaspersky Shield Browser..."

# Clean
rm -rf build dist

# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build -j$(nproc)

# Package
cpack --config build/CPackConfig.cmake

echo "Build complete!"
```

---

## 5. ФРЕЙМВОРКИ И БИБЛИОТЕКИ

### 5.1 Qt Framework (6.8.3)

**Официальный сайт:** https://www.qt.io/  
**Лицензия:** LGPL v3 / Commercial

**Используемые модули:**

```cmake
# CMakeLists.txt
find_package(Qt6 6.8 REQUIRED COMPONENTS
    Core          # Core non-GUI functionality
    Widgets       # Widget classes
    WebEngineCore # Web engine core
    WebEngineWidgets # Web engine widgets
    Network       # Networking
    Sql           # SQL database
    Concurrent    # Multi-threading
)
```

**Примеры использования:**

```cpp
// Qt Widgets
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow() {
        auto* button = new QPushButton("Click me");
        connect(button, &QPushButton::clicked, this, &MainWindow::onButtonClick);
    }
    
private slots:
    void onButtonClick() {
        qDebug() << "Button clicked!";
    }
};
```

```cpp
// Qt WebEngine
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineProfile>

auto* webView = new QWebEngineView();
auto* profile = new QWebEngineProfile("user-profile");
auto* page = new QWebEnginePage(profile, webView);

webView->setPage(page);
webView->load(QUrl("https://google.com"));
```

```cpp
// Qt Network
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

QNetworkAccessManager* manager = new QNetworkAccessManager();
QNetworkRequest request(QUrl("https://api.kaspersky.com/check"));
request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

QNetworkReply* reply = manager->post(request, jsonData);
connect(reply, &QNetworkReply::finished, [reply]() {
    QByteArray response = reply->readAll();
    qDebug() << "Response:" << response;
    reply->deleteLater();
});
```

### 5.2 React (18.2.0)

**Официальный сайт:** https://react.dev/  
**Лицензия:** MIT

**Установка:**
```bash
npm install react@18.2.0 react-dom@18.2.0
```

**Примеры:**

```typescript
// Functional Component with Hooks
import React, { useState, useEffect } from 'react';

export const SecurityDashboard: React.FC = () => {
  const [threats, setThreats] = useState<number>(0);
  
  useEffect(() => {
    const fetchThreats = async () => {
      const response = await fetch('/api/security/threats');
      const data = await response.json();
      setThreats(data.count);
    };
    
    fetchThreats();
  }, []);
  
  return (
    <div>
      <h2>Security Dashboard</h2>
      <p>Threats blocked: {threats}</p>
    </div>
  );
};
```

```typescript
// Context API
import React, { createContext, useContext, useState } from 'react';

interface User {
  id: string;
  email: string;
}

const UserContext = createContext<User | null>(null);

export const UserProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [user, setUser] = useState<User | null>(null);
  
  return (
    <UserContext.Provider value={user}>
      {children}
    </UserContext.Provider>
  );
};

export const useUser = () => {
  const context = useContext(UserContext);
  if (!context) throw new Error('useUser must be used within UserProvider');
  return context;
};
```

### 5.3 Zustand (4.4.7)

**Официальный сайт:** https://zustand-demo.pmnd.rs/  
**Лицензия:** MIT

**State Management:**

```typescript
// stores/browserStore.ts
import { create } from 'zustand';
import { persist } from 'zustand/middleware';

interface Tab {
  id: string;
  title: string;
  url: string;
}

interface BrowserStore {
  tabs: Tab[];
  activeTab: string | null;
  createTab: (url?: string) => void;
  closeTab: (id: string) => void;
  setActiveTab: (id: string) => void;
}

export const useBrowserStore = create<BrowserStore>()(
  persist(
    (set) => ({
      tabs: [],
      activeTab: null,
      
      createTab: (url = 'about:blank') => set((state) => {
        const newTab: Tab = {
          id: crypto.randomUUID(),
          title: 'New Tab',
          url,
        };
        return {
          tabs: [...state.tabs, newTab],
          activeTab: newTab.id,
        };
      }),
      
      closeTab: (id) => set((state) => ({
        tabs: state.tabs.filter(tab => tab.id !== id),
        activeTab: state.activeTab === id ? state.tabs[0]?.id : state.activeTab,
      })),
      
      setActiveTab: (id) => set({ activeTab: id }),
    }),
    { name: 'browser-storage' }
  )
);
```

### 5.4 Express.js (4.18.2)

**Официальный сайт:** https://expressjs.com/  
**Лицензия:** MIT

**Backend Server:**

```typescript
// server/index.ts
import express from 'express';
import cors from 'cors';
import helmet from 'helmet';
import { authRouter } from './routes/auth';
import { securityRouter } from './routes/security';
import { errorHandler } from './middleware/errorHandler';

const app = express();

// Middleware
app.use(helmet());
app.use(cors());
app.use(express.json());

// Routes
app.use('/api/auth', authRouter);
app.use('/api/security', securityRouter);

// Error handling
app.use(errorHandler);

const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
  console.log(`Server running on port ${PORT}`);
});
```

### 5.5 Prisma (5.7.0)

**Официальный сайт:** https://www.prisma.io/  
**Лицензия:** Apache 2.0

**ORM Setup:**

```prisma
// prisma/schema.prisma
datasource db {
  provider = "postgresql"
  url      = env("DATABASE_URL")
}

generator client {
  provider = "prisma-client-js"
}

model User {
  id            String    @id @default(uuid())
  email         String    @unique
  passwordHash  String
  profiles      Profile[]
  createdAt     DateTime  @default(now())
  updatedAt     DateTime  @updatedAt
  
  @@map("users")
}

model Profile {
  id       String  @id @default(uuid())
  userId   String
  name     String
  type     String  // 'parent', 'child', 'teen'
  settings Json?
  
  user User @relation(fields: [userId], references: [id])
  
  @@map("profiles")
}
```

```typescript
// Using Prisma Client
import { PrismaClient } from '@prisma/client';

const prisma = new PrismaClient();

async function createUser(email: string, password: string) {
  const user = await prisma.user.create({
    data: {
      email,
      passwordHash: await hashPassword(password),
      profiles: {
        create: {
          name: 'Default Profile',
          type: 'parent',
        },
      },
    },
    include: {
      profiles: true,
    },
  });
  
  return user;
}
```

### 5.6 Tailwind CSS (3.3.6)

**Официальный сайт:** https://tailwindcss.com/  
**Лицензия:** MIT

**Configuration:**

```javascript
// tailwind.config.js
/** @type {import('tailwindcss').Config} */
export default {
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx}",
  ],
  theme: {
    extend: {
      colors: {
        'kaspersky-green': '#00A88E',
        'kaspersky-dark': '#0D1117',
      },
      fontFamily: {
        sans: ['Inter', 'system-ui', 'sans-serif'],
        mono: ['JetBrains Mono', 'monospace'],
      },
    },
  },
  plugins: [],
}
```

**Usage:**

```tsx
// Using Tailwind classes
export const Button: React.FC = () => {
  return (
    <button className="
      px-4 py-2 
      bg-kaspersky-green 
      text-white 
      rounded-lg 
      hover:bg-kaspersky-green/90 
      transition-colors
    ">
      Click me
    </button>
  );
};
```

### 5.7 Vite (5.0.6)

**Официальный сайт:** https://vitejs.dev/  
**Лицензия:** MIT

**Configuration:**

```typescript
// vite.config.ts
import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react';
import path from 'path';

export default defineConfig({
  plugins: [react()],
  resolve: {
    alias: {
      '@': path.resolve(__dirname, './src'),
      '@components': path.resolve(__dirname, './src/components'),
      '@stores': path.resolve(__dirname, './src/stores'),
    },
  },
  build: {
    outDir: 'dist',
    sourcemap: true,
    rollupOptions: {
      output: {
        manualChunks: {
          'react-vendor': ['react', 'react-dom', 'react-router-dom'],
          'ui-vendor': ['zustand', '@tanstack/react-query'],
        },
      },
    },
  },
  server: {
    port: 3000,
    proxy: {
      '/api': {
        target: 'http://localhost:5000',
        changeOrigin: true,
      },
    },
  },
});
```

### 5.8 Google Test (1.14.0)

**Официальный сайт:** https://github.com/google/googletest  
**Лицензия:** BSD-3-Clause

**C++ Unit Testing:**

```cpp
// tests/SecurityManagerTest.cpp
#include <gtest/gtest.h>
#include "SecurityManager.h"

class SecurityManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        manager = new SecurityManager();
    }
    
    void TearDown() override {
        delete manager;
    }
    
    SecurityManager* manager;
};

TEST_F(SecurityManagerTest, CheckSafeUrl) {
    QString url = "https://google.com";
    auto result = manager->checkUrl(url);
    
    EXPECT_TRUE(result.isSafe);
    EXPECT_EQ(result.threatLevel, 0);
}

TEST_F(SecurityManagerTest, BlockMaliciousUrl) {
    QString url = "https://malicious-site.com";
    auto result = manager->checkUrl(url);
    
    EXPECT_FALSE(result.isSafe);
    EXPECT_GT(result.threatLevel, 0);
}

// Run tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
```

### 5.9 Vitest (1.0.4)

**Официальный сайт:** https://vitest.dev/  
**Лицензия:** MIT

**TypeScript/React Testing:**

```typescript
// tests/browserStore.test.ts
import { describe, it, expect, beforeEach } from 'vitest';
import { renderHook, act } from '@testing-library/react';
import { useBrowserStore } from '@/stores/browserStore';

describe('Browser Store', () => {
  beforeEach(() => {
    // Reset store
    useBrowserStore.setState({ tabs: [], activeTab: null });
  });
  
  it('should create a new tab', () => {
    const { result } = renderHook(() => useBrowserStore());
    
    act(() => {
      result.current.createTab('https://google.com');
    });
    
    expect(result.current.tabs).toHaveLength(1);
    expect(result.current.tabs[0].url).toBe('https://google.com');
    expect(result.current.activeTab).toBe(result.current.tabs[0].id);
  });
  
  it('should close a tab', () => {
    const { result } = renderHook(() => useBrowserStore());
    
    act(() => {
      result.current.createTab();
      result.current.createTab();
    });
    
    const firstTabId = result.current.tabs[0].id;
    
    act(() => {
      result.current.closeTab(firstTabId);
    });
    
    expect(result.current.tabs).toHaveLength(1);
    expect(result.current.tabs.find(t => t.id === firstTabId)).toBeUndefined();
  });
});
```

(продолжение следует из-за ограничения размера...)
