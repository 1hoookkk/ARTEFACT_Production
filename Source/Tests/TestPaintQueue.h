#pragma once
#include "tdd-guard.h"
#include "Core/PaintQueue.h" // Assumes this is the location of your SpscRing

TEST(SpscQueue, OverflowAndOrdering)
{
    // 1. Arrange: Create a queue with a small, testable capacity.
    constexpr int capacity = 16;
    SpscRing<int, capacity> queue;

    // 2. Act: Push more items than the queue can hold to test overflow.
    // We expect only the LAST `capacity` items to remain.
    const int totalToPush = capacity + 5;
    for (int i = 0; i < totalToPush; ++i)
    {
        // Our queue uses forcePush (drop-oldest), so this should never fail.
        queue.forcePush(i);
    }

    // 3. Assert: Verify the contents.
    int expectedValue = 5; // The first value we pushed that should still be in the queue (totalToPush - capacity)
    int poppedCount = 0;
    int actualValue = 0;

    while (queue.pop(actualValue))
    {
        ASSERT_EQ(actualValue, expectedValue);
        expectedValue++;
        poppedCount++;
    }

    // Final check: Did we get the correct number of items?
    ASSERT_EQ(poppedCount, capacity);
}