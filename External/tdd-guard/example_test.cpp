// Example usage of tdd-guard testing framework
// Compile with: g++ -std=c++17 -I External/tdd-guard/include example_test.cpp -o example_test

#include "include/tdd-guard.h"

// Simple test demonstrating basic assertions
TEST(BasicAssertions) {
    ASSERT_TRUE(true);
    ASSERT_FALSE(false);
    ASSERT_EQ(2 + 2, 4);
    ASSERT_NE(5, 3);
    ASSERT_LT(1, 2);
    ASSERT_LE(2, 2);
    ASSERT_GT(3, 2);
    ASSERT_GE(3, 3);
}

// Test demonstrating string comparison
TEST(StringComparison) {
    std::string hello = "hello";
    std::string world = "world";
    
    ASSERT_EQ(hello, "hello");
    ASSERT_NE(hello, world);
}

// Test demonstrating exception handling
TEST(ExceptionHandling) {
    ASSERT_THROW(throw std::runtime_error("test"), std::runtime_error);
    ASSERT_NO_THROW(int x = 42);
}

// Test that intentionally fails to demonstrate failure output
TEST(IntentionalFailure) {
    // Uncomment the line below to see failure output:
    // ASSERT_EQ(1, 2);
    
    // This will pass:
    ASSERT_TRUE(true);
}

int main() {
    return RUN_ALL_TESTS();
}