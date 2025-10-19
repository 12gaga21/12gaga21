# Test configuration
find_package(Qt6 REQUIRED COMPONENTS Test)

# Security Manager Tests
add_executable(test_security_manager
    test_security_manager.cpp
    ../src/SecurityManager.cpp
    ../src/KsnClient.cpp
)

target_link_libraries(test_security_manager
    Qt6::Core
    Qt6::Test
    Qt6::Widgets
    Qt6::Network
    OpenSSL::SSL
    OpenSSL::Crypto
)

target_include_directories(test_security_manager PRIVATE ../include)

# Parental Control Tests
add_executable(test_parental_control
    test_parental_control.cpp
    ../src/ParentalControlManager.cpp
)

target_link_libraries(test_parental_control
    Qt6::Core
    Qt6::Test
    Qt6::Widgets
)

target_include_directories(test_parental_control PRIVATE ../include)

# Database Manager Tests
add_executable(test_database_manager
    test_database_manager.cpp
    ../src/DatabaseManager.cpp
)

target_link_libraries(test_database_manager
    Qt6::Core
    Qt6::Test
    Qt6::Widgets
    Qt6::Sql
    SQLite::SQLite3
)

target_include_directories(test_database_manager PRIVATE ../include)

# Profile Manager Tests
add_executable(test_profile_manager
    test_profile_manager.cpp
    ../src/ProfileManager.cpp
)

target_link_libraries(test_profile_manager
    Qt6::Core
    Qt6::Test
    Qt6::Widgets
)

target_include_directories(test_profile_manager PRIVATE ../include)

# Add tests to CTest
add_test(NAME SecurityManagerTests COMMAND test_security_manager)
add_test(NAME ParentalControlTests COMMAND test_parental_control)
add_test(NAME DatabaseManagerTests COMMAND test_database_manager)
add_test(NAME ProfileManagerTests COMMAND test_profile_manager)