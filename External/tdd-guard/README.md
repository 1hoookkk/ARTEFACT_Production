# TDD-Guard: Minimal C++ Testing Framework

A single-header, dependency-free testing framework for C++17 inspired by the nizos/tdd-guard project.

## Features

- **Single Header**: Include `tdd-guard.h` and you're ready to go
- **C++17 Compatible**: Works with modern C++ standards
- **Minimal Dependencies**: Only uses standard library components
- **Simple API**: Easy-to-use macros for test definition and assertions
- **Automatic Registration**: Tests register themselves automatically
- **Console Output**: Clear pass/fail reporting with detailed error messages

## Quick Start

1. Include the header:
```cpp
#include "External/tdd-guard/include/tdd-guard.h"
```

2. Define tests using the `TEST` macro:
```cpp
TEST(MyTestName) {
    ASSERT_EQ(2 + 2, 4);
    ASSERT_TRUE(true);
    ASSERT_FALSE(false);
}
```

3. Run all tests in your main function:
```cpp
int main() {
    return RUN_ALL_TESTS();
}
```

## Available Assertions

| Macro | Description |
|-------|-------------|
| `ASSERT_TRUE(condition)` | Asserts that condition is true |
| `ASSERT_FALSE(condition)` | Asserts that condition is false |
| `ASSERT_EQ(expected, actual)` | Asserts equality |
| `ASSERT_NE(expected, actual)` | Asserts inequality |
| `ASSERT_LT(a, b)` | Asserts a < b |
| `ASSERT_LE(a, b)` | Asserts a <= b |
| `ASSERT_GT(a, b)` | Asserts a > b |
| `ASSERT_GE(a, b)` | Asserts a >= b |
| `ASSERT_THROW(statement, exception_type)` | Asserts that statement throws specific exception |
| `ASSERT_NO_THROW(statement)` | Asserts that statement doesn't throw |

## Example

See `example_test.cpp` for a complete working example.

## Building Tests

```bash
# Compile a test file
g++ -std=c++17 -I External/tdd-guard/include your_test.cpp -o your_test

# Run the test
./your_test
```

## Verification

Run the verification script to ensure proper installation:

```bash
chmod +x scripts/verify-tdd-guard.sh
./scripts/verify-tdd-guard.sh
```

## Integration with Project

This framework is designed to integrate seamlessly with the SpectralCanvas Pro project's existing test infrastructure while providing a lightweight alternative to larger testing frameworks.