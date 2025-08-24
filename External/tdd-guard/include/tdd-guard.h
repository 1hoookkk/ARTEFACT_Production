#pragma once

//
// TDD-Guard: Lightweight C++ Testing Framework
// Single-header, dependency-free testing framework for C++17
// Inspired by doctest and Catch2
// License: MIT
//

#ifndef TDD_GUARD_H
#define TDD_GUARD_H

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <sstream>
#include <exception>
#include <memory>
#include <chrono>
#include <iomanip>
#include <map>

namespace tdd_guard {

// Forward declarations
class TestCase;
class TestRunner;
class TestResult;

// Configuration options
struct Config {
    bool verbose = false;
    bool color_output = true;
    bool timing = true;
    bool abort_on_first_failure = false;
    std::string filter = "";
    
    static Config& instance() {
        static Config config;
        return config;
    }
};

// Test result tracking
struct TestResult {
    std::string name;
    std::string suite;
    bool passed;
    std::string error_message;
    std::string file;
    int line;
    double duration_ms;
    
    TestResult() : passed(false), line(0), duration_ms(0.0) {}
};

// Exception for test failures
class AssertionFailure : public std::exception {
private:
    std::string message_;
    std::string file_;
    int line_;
    
public:
    AssertionFailure(const std::string& message, const char* file, int line) 
        : message_(message), file_(file), line_(line) {}
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
    const std::string& file() const { return file_; }
    int line() const { return line_; }
};

// Test case wrapper
class TestCase {
public:
    std::string name;
    std::string suite;
    std::string file;
    int line;
    std::function<void()> test_function;
    
    TestCase(const std::string& suite_name, const std::string& test_name, 
             const std::string& file_name, int line_num, std::function<void()> func)
        : name(test_name), suite(suite_name), file(file_name), line(line_num), test_function(func) {}
};

// Main test runner
class TestRunner {
private:
    std::vector<std::unique_ptr<TestCase>> test_cases_;
    std::vector<TestResult> results_;
    std::string current_test_name_;
    std::string current_suite_;
    
    // Color output helpers
    std::string red(const std::string& text) const {
        return Config::instance().color_output ? "\033[31m" + text + "\033[0m" : text;
    }
    
    std::string green(const std::string& text) const {
        return Config::instance().color_output ? "\033[32m" + text + "\033[0m" : text;
    }
    
    std::string yellow(const std::string& text) const {
        return Config::instance().color_output ? "\033[33m" + text + "\033[0m" : text;
    }
    
    std::string bold(const std::string& text) const {
        return Config::instance().color_output ? "\033[1m" + text + "\033[0m" : text;
    }
    
public:
    static TestRunner& instance() {
        static TestRunner runner;
        return runner;
    }
    
    void register_test(const std::string& suite, const std::string& name, 
                      const std::string& file, int line, std::function<void()> test_func) {
        auto test_case = std::make_unique<TestCase>(suite, name, file, line, test_func);
        test_cases_.push_back(std::move(test_case));
    }
    
    bool should_run_test(const TestCase& test) const {
        if (Config::instance().filter.empty()) {
            return true;
        }
        
        std::string full_name = test.suite + "::" + test.name;
        return full_name.find(Config::instance().filter) != std::string::npos;
    }
    
    int run_all_tests() {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        if (Config::instance().verbose) {
            std::cout << bold("TDD-Guard Test Runner") << std::endl;
            std::cout << "=====================" << std::endl;
        }
        
        int tests_run = 0;
        int tests_passed = 0;
        int tests_failed = 0;
        
        for (const auto& test_case : test_cases_) {
            if (!should_run_test(*test_case)) {
                continue;
            }
            
            tests_run++;
            current_test_name_ = test_case->name;
            current_suite_ = test_case->suite;
            
            TestResult result;
            result.name = test_case->name;
            result.suite = test_case->suite;
            result.file = test_case->file;
            result.line = test_case->line;
            
            auto test_start = std::chrono::high_resolution_clock::now();
            
            try {
                test_case->test_function();
                result.passed = true;
                tests_passed++;
                
                if (Config::instance().verbose) {
                    std::cout << green("[PASS]") << " " << result.suite << "::" << result.name;
                }
                
            } catch (const AssertionFailure& e) {
                result.passed = false;
                result.error_message = e.what();
                tests_failed++;
                
                std::cout << red("[FAIL]") << " " << result.suite << "::" << result.name << std::endl;
                std::cout << "  " << e.what() << std::endl;
                std::cout << "  at " << e.file() << ":" << e.line() << std::endl;
                
                if (Config::instance().abort_on_first_failure) {
                    break;
                }
                
            } catch (const std::exception& e) {
                result.passed = false;
                result.error_message = std::string("Unexpected exception: ") + e.what();
                tests_failed++;
                
                std::cout << red("[FAIL]") << " " << result.suite << "::" << result.name << std::endl;
                std::cout << "  Unexpected exception: " << e.what() << std::endl;
                
                if (Config::instance().abort_on_first_failure) {
                    break;
                }
                
            } catch (...) {
                result.passed = false;
                result.error_message = "Unknown exception";
                tests_failed++;
                
                std::cout << red("[FAIL]") << " " << result.suite << "::" << result.name << std::endl;
                std::cout << "  Unknown exception" << std::endl;
                
                if (Config::instance().abort_on_first_failure) {
                    break;
                }
            }
            
            auto test_end = std::chrono::high_resolution_clock::now();
            result.duration_ms = std::chrono::duration<double, std::milli>(test_end - test_start).count();
            
            if (Config::instance().verbose && result.passed && Config::instance().timing) {
                std::cout << " (" << std::fixed << std::setprecision(2) << result.duration_ms << "ms)" << std::endl;
            }
            
            results_.push_back(result);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        double total_duration = std::chrono::duration<double, std::milli>(end_time - start_time).count();
        
        // Summary
        std::cout << std::endl;
        std::cout << bold("Test Summary:") << std::endl;
        std::cout << "=============" << std::endl;
        std::cout << "Tests run: " << tests_run << std::endl;
        std::cout << green("Passed: ") << tests_passed << std::endl;
        
        if (tests_failed > 0) {
            std::cout << red("Failed: ") << tests_failed << std::endl;
        } else {
            std::cout << "Failed: 0" << std::endl;
        }
        
        if (Config::instance().timing) {
            std::cout << "Total time: " << std::fixed << std::setprecision(2) << total_duration << "ms" << std::endl;
        }
        
        if (tests_failed == 0 && tests_run > 0) {
            std::cout << std::endl << green("All tests passed!") << std::endl;
        }
        
        return tests_failed > 0 ? 1 : 0;
    }
    
    void clear() {
        test_cases_.clear();
        results_.clear();
    }
    
    const std::string& current_test_name() const {
        return current_test_name_;
    }
    
    const std::string& current_suite() const {
        return current_suite_;
    }
    
    const std::vector<TestResult>& results() const {
        return results_;
    }
    
    size_t test_count() const {
        return test_cases_.size();
    }
};

// Helper class for test registration
class TestRegistrar {
public:
    TestRegistrar(const std::string& suite, const std::string& name, 
                  const std::string& file, int line, std::function<void()> test_func) {
        TestRunner::instance().register_test(suite, name, file, line, test_func);
    }
};

// String comparison helper
template<typename T>
std::string stringify(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

template<>
std::string stringify(const std::string& value) {
    return "\"" + value + "\"";
}

template<>
std::string stringify(const char* const& value) {
    return std::string("\"") + value + "\"";
}

} // namespace tdd_guard

// Assertion macros
#define TDD_ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            std::ostringstream oss; \
            oss << "ASSERT_TRUE failed: " << #condition; \
            throw tdd_guard::AssertionFailure(oss.str(), __FILE__, __LINE__); \
        } \
    } while (0)

#define TDD_ASSERT_FALSE(condition) \
    do { \
        if (condition) { \
            std::ostringstream oss; \
            oss << "ASSERT_FALSE failed: " << #condition; \
            throw tdd_guard::AssertionFailure(oss.str(), __FILE__, __LINE__); \
        } \
    } while (0)

#define TDD_ASSERT_EQ(expected, actual) \
    do { \
        if (!((expected) == (actual))) { \
            std::ostringstream oss; \
            oss << "ASSERT_EQ failed: expected " << tdd_guard::stringify(expected) \
                << ", got " << tdd_guard::stringify(actual); \
            throw tdd_guard::AssertionFailure(oss.str(), __FILE__, __LINE__); \
        } \
    } while (0)

#define TDD_ASSERT_NE(expected, actual) \
    do { \
        if ((expected) == (actual)) { \
            std::ostringstream oss; \
            oss << "ASSERT_NE failed: values are equal: " << tdd_guard::stringify(actual); \
            throw tdd_guard::AssertionFailure(oss.str(), __FILE__, __LINE__); \
        } \
    } while (0)

#define TDD_ASSERT_LT(a, b) \
    do { \
        if (!((a) < (b))) { \
            std::ostringstream oss; \
            oss << "ASSERT_LT failed: " << tdd_guard::stringify(a) << " >= " << tdd_guard::stringify(b); \
            throw tdd_guard::AssertionFailure(oss.str(), __FILE__, __LINE__); \
        } \
    } while (0)

#define TDD_ASSERT_LE(a, b) \
    do { \
        if (!((a) <= (b))) { \
            std::ostringstream oss; \
            oss << "ASSERT_LE failed: " << tdd_guard::stringify(a) << " > " << tdd_guard::stringify(b); \
            throw tdd_guard::AssertionFailure(oss.str(), __FILE__, __LINE__); \
        } \
    } while (0)

#define TDD_ASSERT_GT(a, b) \
    do { \
        if (!((a) > (b))) { \
            std::ostringstream oss; \
            oss << "ASSERT_GT failed: " << tdd_guard::stringify(a) << " <= " << tdd_guard::stringify(b); \
            throw tdd_guard::AssertionFailure(oss.str(), __FILE__, __LINE__); \
        } \
    } while (0)

#define TDD_ASSERT_GE(a, b) \
    do { \
        if (!((a) >= (b))) { \
            std::ostringstream oss; \
            oss << "ASSERT_GE failed: " << tdd_guard::stringify(a) << " < " << tdd_guard::stringify(b); \
            throw tdd_guard::AssertionFailure(oss.str(), __FILE__, __LINE__); \
        } \
    } while (0)

#define TDD_ASSERT_NEAR(a, b, tolerance) \
    do { \
        auto diff = ((a) > (b)) ? ((a) - (b)) : ((b) - (a)); \
        if (diff > (tolerance)) { \
            std::ostringstream oss; \
            oss << "ASSERT_NEAR failed: " << tdd_guard::stringify(a) << " and " \
                << tdd_guard::stringify(b) << " differ by " << diff \
                << ", which exceeds tolerance " << tdd_guard::stringify(tolerance); \
            throw tdd_guard::AssertionFailure(oss.str(), __FILE__, __LINE__); \
        } \
    } while (0)

#define TDD_ASSERT_THROW(statement, exception_type) \
    do { \
        bool threw_expected = false; \
        try { \
            statement; \
        } catch (const exception_type&) { \
            threw_expected = true; \
        } catch (...) { \
            std::ostringstream oss; \
            oss << "ASSERT_THROW failed: wrong exception type thrown from: " << #statement; \
            throw tdd_guard::AssertionFailure(oss.str(), __FILE__, __LINE__); \
        } \
        if (!threw_expected) { \
            std::ostringstream oss; \
            oss << "ASSERT_THROW failed: no exception thrown from: " << #statement; \
            throw tdd_guard::AssertionFailure(oss.str(), __FILE__, __LINE__); \
        } \
    } while (0)

#define TDD_ASSERT_NO_THROW(statement) \
    do { \
        try { \
            statement; \
        } catch (...) { \
            std::ostringstream oss; \
            oss << "ASSERT_NO_THROW failed: exception thrown from: " << #statement; \
            throw tdd_guard::AssertionFailure(oss.str(), __FILE__, __LINE__); \
        } \
    } while (0)

// Test definition macros
#define TDD_TEST_CASE(suite_name, test_name) \
    void test_##suite_name##_##test_name##_impl(); \
    static tdd_guard::TestRegistrar test_##suite_name##_##test_name##_registrar( \
        #suite_name, #test_name, __FILE__, __LINE__, test_##suite_name##_##test_name##_impl); \
    void test_##suite_name##_##test_name##_impl()

#define TDD_TEST(test_name) TDD_TEST_CASE(Default, test_name)

// Convenience macro for running all tests in main()
#define TDD_RUN_ALL_TESTS() tdd_guard::TestRunner::instance().run_all_tests()

// Optional: Clear all tests (useful for testing the framework itself)
#define TDD_CLEAR_ALL_TESTS() tdd_guard::TestRunner::instance().clear()

// Configuration macros
#define TDD_CONFIG_VERBOSE(enable) tdd_guard::Config::instance().verbose = (enable)
#define TDD_CONFIG_COLOR(enable) tdd_guard::Config::instance().color_output = (enable)
#define TDD_CONFIG_TIMING(enable) tdd_guard::Config::instance().timing = (enable)
#define TDD_CONFIG_ABORT_ON_FAIL(enable) tdd_guard::Config::instance().abort_on_first_failure = (enable)
#define TDD_CONFIG_FILTER(pattern) tdd_guard::Config::instance().filter = (pattern)

// Backwards compatibility aliases
#define ASSERT_TRUE TDD_ASSERT_TRUE
#define ASSERT_FALSE TDD_ASSERT_FALSE
#define ASSERT_EQ TDD_ASSERT_EQ
#define ASSERT_NE TDD_ASSERT_NE
#define ASSERT_LT TDD_ASSERT_LT
#define ASSERT_LE TDD_ASSERT_LE
#define ASSERT_GT TDD_ASSERT_GT
#define ASSERT_GE TDD_ASSERT_GE
#define ASSERT_NEAR TDD_ASSERT_NEAR
#define ASSERT_THROW TDD_ASSERT_THROW
#define ASSERT_NO_THROW TDD_ASSERT_NO_THROW
#define TEST_CASE TDD_TEST_CASE
#define TEST TDD_TEST
#define RUN_ALL_TESTS TDD_RUN_ALL_TESTS
#define CLEAR_ALL_TESTS TDD_CLEAR_ALL_TESTS

#endif // TDD_GUARD_H

#ifdef TDD_GUARD_IMPLEMENTATION
// Implementation is already included in the header above
// This section is for compatibility with single-header conventions
#endif