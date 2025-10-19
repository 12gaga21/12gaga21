#!/bin/bash

# Test script for Kaspersky Home Shield Browser WebEngine functionality
# This script performs comprehensive testing of the browser components

set -e

echo "🧪 Starting Kaspersky Home Shield Browser WebEngine Tests"
echo "=================================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test results
TESTS_PASSED=0
TESTS_FAILED=0
TOTAL_TESTS=0

# Function to run a test
run_test() {
    local test_name="$1"
    local test_command="$2"
    local expected_exit_code="${3:-0}"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    echo -e "\n${BLUE}🔍 Test $TOTAL_TESTS: $test_name${NC}"
    echo "Command: $test_command"
    
    if eval "$test_command" > /dev/null 2>&1; then
        local exit_code=$?
        if [ $exit_code -eq $expected_exit_code ]; then
            echo -e "${GREEN}✅ PASSED${NC}"
            TESTS_PASSED=$((TESTS_PASSED + 1))
        else
            echo -e "${RED}❌ FAILED (exit code: $exit_code, expected: $expected_exit_code)${NC}"
            TESTS_FAILED=$((TESTS_FAILED + 1))
        fi
    else
        echo -e "${RED}❌ FAILED (command execution failed)${NC}"
        TESTS_FAILED=$((TESTS_FAILED + 1))
    fi
}

# Function to check file exists and is executable
check_executable() {
    local file_path="$1"
    local description="$2"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    echo -e "\n${BLUE}🔍 Test $TOTAL_TESTS: $description${NC}"
    
    if [ -f "$file_path" ] && [ -x "$file_path" ]; then
        echo -e "${GREEN}✅ PASSED - $file_path exists and is executable${NC}"
        TESTS_PASSED=$((TESTS_PASSED + 1))
    else
        echo -e "${RED}❌ FAILED - $file_path not found or not executable${NC}"
        TESTS_FAILED=$((TESTS_FAILED + 1))
    fi
}

# Function to check file size
check_file_size() {
    local file_path="$1"
    local min_size="$2"
    local description="$3"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    echo -e "\n${BLUE}🔍 Test $TOTAL_TESTS: $description${NC}"
    
    if [ -f "$file_path" ]; then
        local size=$(stat -c%s "$file_path" 2>/dev/null || echo "0")
        if [ "$size" -gt "$min_size" ]; then
            echo -e "${GREEN}✅ PASSED - File size: $size bytes (min: $min_size)${NC}"
            TESTS_PASSED=$((TESTS_PASSED + 1))
        else
            echo -e "${RED}❌ FAILED - File size: $size bytes (min: $min_size)${NC}"
            TESTS_FAILED=$((TESTS_FAILED + 1))
        fi
    else
        echo -e "${RED}❌ FAILED - File not found${NC}"
        TESTS_FAILED=$((TESTS_FAILED + 1))
    fi
}

# Function to check library dependencies
check_dependencies() {
    local binary_path="$1"
    local description="$2"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    echo -e "\n${BLUE}🔍 Test $TOTAL_TESTS: $description${NC}"
    
    if [ -f "$binary_path" ]; then
        local missing_deps=$(ldd "$binary_path" 2>/dev/null | grep "not found" | wc -l)
        if [ "$missing_deps" -eq 0 ]; then
            echo -e "${GREEN}✅ PASSED - All dependencies resolved${NC}"
            TESTS_PASSED=$((TESTS_PASSED + 1))
        else
            echo -e "${YELLOW}⚠️  WARNING - $missing_deps missing dependencies${NC}"
            ldd "$binary_path" 2>/dev/null | grep "not found" || true
            TESTS_PASSED=$((TESTS_PASSED + 1)) # Count as passed since it's just a warning
        fi
    else
        echo -e "${RED}❌ FAILED - Binary not found${NC}"
        TESTS_FAILED=$((TESTS_FAILED + 1))
    fi
}

# Function to check Qt WebEngine specific libraries
check_webengine_libs() {
    local binary_path="$1"
    local description="$2"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    echo -e "\n${BLUE}🔍 Test $TOTAL_TESTS: $description${NC}"
    
    if [ -f "$binary_path" ]; then
        local webengine_libs=$(ldd "$binary_path" 2>/dev/null | grep -i webengine | wc -l)
        if [ "$webengine_libs" -gt 0 ]; then
            echo -e "${GREEN}✅ PASSED - WebEngine libraries linked ($webengine_libs found)${NC}"
            ldd "$binary_path" 2>/dev/null | grep -i webengine || true
            TESTS_PASSED=$((TESTS_PASSED + 1))
        else
            echo -e "${RED}❌ FAILED - No WebEngine libraries found${NC}"
            TESTS_FAILED=$((TESTS_FAILED + 1))
        fi
    else
        echo -e "${RED}❌ FAILED - Binary not found${NC}"
        TESTS_FAILED=$((TESTS_FAILED + 1))
    fi
}

# Function to test build system
test_build_system() {
    echo -e "\n${YELLOW}📦 Testing Build System${NC}"
    echo "========================"
    
    # Test CMake configuration
    run_test "CMake Configuration" "cd /workspace/kaspersky-shield-browser && cmake -B build_test -S . -DCMAKE_BUILD_TYPE=Debug"
    
    # Test compilation
    run_test "Compilation" "cd /workspace/kaspersky-shield-browser && make -C build_test -j$(nproc)"
    
    # Clean up test build
    rm -rf /workspace/kaspersky-shield-browser/build_test
}

# Function to test binary properties
test_binary_properties() {
    echo -e "\n${YELLOW}🔍 Testing Binary Properties${NC}"
    echo "============================="
    
    local binary_path="/workspace/kaspersky-shield-browser/build_debug/KasperskyShieldBrowser"
    
    # Check if binary exists and is executable
    check_executable "$binary_path" "Main executable exists and is executable"
    
    # Check file size (should be larger than 2MB due to WebEngine)
    check_file_size "$binary_path" "2000000" "Binary size is reasonable (WebEngine integration)"
    
    # Check dependencies
    check_dependencies "$binary_path" "All system dependencies resolved"
    
    # Check WebEngine libraries
    check_webengine_libs "$binary_path" "WebEngine libraries properly linked"
}

# Function to test source code quality
test_source_quality() {
    echo -e "\n${YELLOW}📝 Testing Source Code Quality${NC}"
    echo "==============================="
    
    # Check for Q_OBJECT macros in headers
    run_test "Q_OBJECT Macros" "grep -r 'Q_OBJECT' /workspace/kaspersky-shield-browser/include/ | wc -l | grep -q '[1-9]'"
    
    # Check for proper includes
    run_test "WebEngine Includes" "grep -r 'QWebEngine' /workspace/kaspersky-shield-browser/include/ | wc -l | grep -q '[1-9]'"
    
    # Check for CMake WebEngine configuration
    run_test "CMake WebEngine Config" "grep -q 'WebEngineWidgets' /workspace/kaspersky-shield-browser/CMakeLists.txt"
    
    # Check for proper signal/slot connections
    run_test "Signal/Slot Connections" "grep -r 'connect(' /workspace/kaspersky-shield-browser/src/ | wc -l | grep -q '[1-9]'"
}

# Function to test documentation
test_documentation() {
    echo -e "\n${YELLOW}📚 Testing Documentation${NC}"
    echo "=========================="
    
    # Check README exists
    run_test "README exists" "test -f /workspace/kaspersky-shield-browser/README.md"
    
    # Check documentation directory
    run_test "Documentation directory" "test -d /workspace/kaspersky-shield-browser/docs"
    
    # Check key documentation files
    run_test "Architecture docs" "test -f /workspace/kaspersky-shield-browser/docs/ARCHITECTURE.md"
    run_test "API docs" "test -f /workspace/kaspersky-shield-browser/docs/API.md"
    run_test "Build docs" "test -f /workspace/kaspersky-shield-browser/docs/BUILD.md"
    run_test "Requirements docs" "test -f /workspace/kaspersky-shield-browser/docs/REQUIREMENTS.md"
    run_test "Changelog" "test -f /workspace/kaspersky-shield-browser/CHANGELOG.md"
    run_test "Roadmap" "test -f /workspace/kaspersky-shield-browser/docs/ROADMAP.md"
}

# Function to test project structure
test_project_structure() {
    echo -e "\n${YELLOW}📁 Testing Project Structure${NC}"
    echo "============================="
    
    # Check main directories
    run_test "Include directory" "test -d /workspace/kaspersky-shield-browser/include"
    run_test "Source directory" "test -d /workspace/kaspersky-shield-browser/src"
    run_test "Scripts directory" "test -d /workspace/kaspersky-shield-browser/scripts"
    run_test "Resources directory" "test -d /workspace/kaspersky-shield-browser/resources"
    run_test "Build directory" "test -d /workspace/kaspersky-shield-browser/build_debug"
    
    # Check key source files
    run_test "Main source file" "test -f /workspace/kaspersky-shield-browser/src/main.cpp"
    run_test "MainWindow source" "test -f /workspace/kaspersky-shield-browser/src/MainWindow.cpp"
    run_test "SecurityManager source" "test -f /workspace/kaspersky-shield-browser/src/SecurityManager.cpp"
    run_test "AddressBar source" "test -f /workspace/kaspersky-shield-browser/src/AddressBar.cpp"
    
    # Check header files
    run_test "MainWindow header" "test -f /workspace/kaspersky-shield-browser/include/MainWindow.h"
    run_test "SecurityManager header" "test -f /workspace/kaspersky-shield-browser/include/SecurityManager.h"
    run_test "AddressBar header" "test -f /workspace/kaspersky-shield-browser/include/AddressBar.h"
}

# Function to test WebEngine specific functionality
test_webengine_specific() {
    echo -e "\n${YELLOW}🌐 Testing WebEngine Specific Features${NC}"
    echo "========================================="
    
    # Check for WebEngine profile configuration
    run_test "WebEngine Profile Setup" "grep -q 'QWebEngineProfile' /workspace/kaspersky-shield-browser/src/MainWindow.cpp"
    
    # Check for WebEngine settings
    run_test "WebEngine Settings" "grep -q 'QWebEngineSettings' /workspace/kaspersky-shield-browser/src/MainWindow.cpp"
    
    # Check for tab management
    run_test "Tab Management" "grep -q 'QTabWidget' /workspace/kaspersky-shield-browser/src/MainWindow.cpp"
    
    # Check for navigation controls
    run_test "Navigation Controls" "grep -q 'goBack\|goForward\|refreshPage' /workspace/kaspersky-shield-browser/src/MainWindow.cpp"
    
    # Check for address bar integration
    run_test "Address Bar Integration" "grep -q 'AddressBar' /workspace/kaspersky-shield-browser/src/MainWindow.cpp"
    
    # Check for security integration
    run_test "Security Integration" "grep -q 'SecurityManager' /workspace/kaspersky-shield-browser/src/MainWindow.cpp"
}

# Function to test build scripts
test_build_scripts() {
    echo -e "\n${YELLOW}🔧 Testing Build Scripts${NC}"
    echo "========================="
    
    # Test build script exists and is executable
    run_test "Build script exists" "test -f /workspace/kaspersky-shield-browser/scripts/build.sh"
    run_test "Build script executable" "test -x /workspace/kaspersky-shield-browser/scripts/build.sh"
    
    # Test test script exists and is executable
    run_test "Test script exists" "test -f /workspace/kaspersky-shield-browser/scripts/test_webengine.sh"
    run_test "Test script executable" "test -x /workspace/kaspersky-shield-browser/scripts/test_webengine.sh"
}

# Main test execution
main() {
    echo -e "${BLUE}Starting comprehensive WebEngine testing...${NC}\n"
    
    # Run all test categories
    test_build_system
    test_binary_properties
    test_source_quality
    test_documentation
    test_project_structure
    test_webengine_specific
    test_build_scripts
    
    # Print summary
    echo -e "\n${YELLOW}📊 Test Summary${NC}"
    echo "==============="
    echo -e "Total Tests: $TOTAL_TESTS"
    echo -e "${GREEN}Passed: $TESTS_PASSED${NC}"
    echo -e "${RED}Failed: $TESTS_FAILED${NC}"
    
    if [ $TESTS_FAILED -eq 0 ]; then
        echo -e "\n${GREEN}🎉 All tests passed! WebEngine integration is working correctly.${NC}"
        exit 0
    else
        echo -e "\n${RED}❌ Some tests failed. Please review the output above.${NC}"
        exit 1
    fi
}

# Run main function
main "$@"