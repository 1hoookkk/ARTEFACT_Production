// SampleTest.cpp - Basic scaffolding test for harness verification
// Created: 2025-08-16
// Purpose: Trivial passing test to verify tdd-guard integration works
// NOTE: This file can be deleted after harness verification is complete

#include "../../External/tdd-guard/include/tdd-guard.h"

// Simple test to verify the test harness boots and runs correctly
TEST(harness_boots) {
    // This test always passes - it's just to verify the framework works
    ASSERT_TRUE(true);
}