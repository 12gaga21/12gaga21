cmake_minimum_required(VERSION 3.20)
project(KasperskyShieldBrowser VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Network Sql WebEngineWidgets)
find_package(OpenSSL REQUIRED)
find_package(SQLite3 REQUIRED)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/SecurityManager.cpp
    src/ParentalControlManager.cpp
    src/DatabaseManager.cpp
    src/KsnClient.cpp
    src/AddressBar.cpp
    src/ProfileManager.cpp
)

# Header files
set(HEADERS
    include/MainWindow.h
    include/SecurityManager.h
    include/ParentalControlManager.h
    include/DatabaseManager.h
    include/KsnClient.h
    include/AddressBar.h
    include/ProfileManager.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::Network
    Qt6::Sql
    Qt6::WebEngineWidgets
    OpenSSL::SSL
    OpenSSL::Crypto
    SQLite::SQLite3
)

# Set target properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# Copy resources
file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Testing
enable_testing()
add_subdirectory(tests)