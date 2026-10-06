#include "Diagnostics.h"
#include <Arduino.h>
#include "Config.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_memory_types.h"

extern "C" {
extern int _data_start;
extern int _bss_end;
}

static const char *Region(const void *address) {
    const uint8_t *pointer = static_cast<const uint8_t *>(address);
    const uint8_t *staticStart = reinterpret_cast<const uint8_t *>(&_data_start);
    const uint8_t *staticEnd = reinterpret_cast<const uint8_t *>(&_bss_end);
    const uint8_t *stackStart = pxTaskGetStackStart(nullptr);

    if (pointer >= staticStart && pointer < staticEnd) {
        return "static memory (globals)";
    }
    if (pointer >= stackStart && pointer < stackStart + CONFIG_ARDUINO_LOOP_STACK_SIZE) {
        return "stack (loop task)";
    }
    if (esp_ptr_external_ram(address)) {
        return "heap, external PSRAM";
    }
    if (esp_ptr_in_dram(address)) {
        return "heap, internal RAM";
    }
    return "flash (read-only)";
}

void Diagnostics::PrintMemory(const char *name, const void *address, size_t size) {
    Serial.printf("%-22s %p  %6u bytes  %s\n", name, address, static_cast<unsigned>(size), Region(address));
}

void Diagnostics::PrintFreeMemory() {
    Serial.printf("free heap: internal %u bytes, PSRAM %u bytes | loop stack never used: %u bytes\n",
                  static_cast<unsigned>(heap_caps_get_free_size(MALLOC_CAP_INTERNAL)),
                  static_cast<unsigned>(heap_caps_get_free_size(MALLOC_CAP_SPIRAM)),
                  static_cast<unsigned>(uxTaskGetStackHighWaterMark(nullptr)));
}

void Diagnostics::BeginLoop() {
    loopStartUs = micros();
}

void Diagnostics::EndLoop(uint32_t now) {
    if (lastReportMs == 0) {
        lastReportMs = now;
    }
    const uint32_t loopUs = micros() - loopStartUs;
    loopCount++;
    totalLoopUs += loopUs;
    if (loopUs > longestLoopUs) {
        longestLoopUs = loopUs;
    }

    if (now - lastReportMs < Debug::REPORT_EVERY_MS) {
        return;
    }
    Serial.printf("\033[K%u loops/s | average loop %u us | longest loop %u us\n",
                  static_cast<unsigned>(loopCount * 1000 / (now - lastReportMs)),
                  static_cast<unsigned>(totalLoopUs / loopCount),
                  static_cast<unsigned>(longestLoopUs));
    Serial.print("\033[K");
    PrintFreeMemory();

    lastReportMs = now;
    loopCount = 0;
    totalLoopUs = 0;
    longestLoopUs = 0;
}
