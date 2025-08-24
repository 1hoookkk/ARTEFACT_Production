#include <JuceHeader.h>
#include "Core/PaintQueue.h"
#include <cassert>
#include <iostream>

static int failures = 0;

template <typename T>
static void expectTrue(const T& cond, const char* msg)
{
    if (!cond) {
        ++failures;
        std::cerr << "FAIL: " << msg << "\n";
    }
}

int main()
{
    {
        // Basic push/pop order
        PaintQueue<int, 8> q;
        expectTrue(q.empty(), "Queue should start empty");
        expectTrue(q.push(1), "push 1");
        expectTrue(q.push(2), "push 2");
        int v = 0;
        expectTrue(q.pop(v), "pop 1");
        expectTrue(v == 1, "value 1");
        expectTrue(q.pop(v), "pop 2");
        expectTrue(v == 2, "value 2");
        expectTrue(q.empty(), "empty after pops");
    }

    {
        // Wrap-around correctness and capacity semantics (Capacity-1 usable)
        PaintQueue<int, 8> q;
        for (int i = 0; i < 7; ++i)
            expectTrue(q.push(100 + i), "wrap push");
        // Next push should fail (full)
        expectTrue(!q.push(999), "full refuses push");
        int v = 0;
        for (int i = 0; i < 3; ++i) {
            expectTrue(q.pop(v), "pop wrap");
        }
        // Now we have room for 3 items
        expectTrue(q.push(201), "push after pop 1");
        expectTrue(q.push(202), "push after pop 2");
        expectTrue(q.push(203), "push after pop 3");
    }

    {
        // Overflow behavior on UI ring (SpectralSynthEngine style) isn't tested here – this queue refuses push when full
        PaintQueue<int, 4> q;
        expectTrue(q.push(1), "push1");
        expectTrue(q.push(2), "push2");
        expectTrue(q.push(3), "push3");
        expectTrue(!q.push(4), "reject 4 when full");
        int v=0; q.pop(v); q.pop(v);
        expectTrue(q.push(4), "push 4 after space");
    }

    if (failures == 0)
        std::cout << "All PaintQueue tests passed\n";
    return failures;
}

