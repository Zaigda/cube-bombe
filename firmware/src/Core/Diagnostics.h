#pragma once
#include <cstddef>
#include <cstdint>

class Diagnostics {
public:
    static void PrintMemory(const char *name, const void *address, size_t size);

    static void PrintFreeMemory();

    void BeginLoop();

    void EndLoop(uint32_t now);

private:
    uint32_t loopStartUs = 0;
    uint32_t loopCount = 0;
    uint32_t totalLoopUs = 0;
    uint32_t longestLoopUs = 0;
    uint32_t lastReportMs = 0;
};
