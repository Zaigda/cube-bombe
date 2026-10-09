#pragma once
#include <Arduino.h>
#include <cstring>
#include "Common/Config.h"

// Sends a message struct over a serial wire as START_BYTE, the struct's bytes, then a checksum, so the reader
// finds the start again if a byte is lost. Both boards must use the same struct.
namespace MessageLink {
    constexpr uint8_t START_BYTE = 0xA5;

    inline uint8_t Checksum(const uint8_t *bytes, size_t size) {
        uint8_t sum = 0;
        for (size_t i = 0; i < size; i++) {
            sum += bytes[i];
        }
        return sum ^ 0xFF;
    }

    template <typename T>
    void Send(HardwareSerial &serial, const T &message) {
        const uint8_t *bytes = reinterpret_cast<const uint8_t *>(&message);
        serial.write(START_BYTE);
        serial.write(bytes, sizeof(T));
        serial.write(Checksum(bytes, sizeof(T)));
    }
}

// Reads messages of type T without blocking. Receive() copies each whole, valid message into message, and empties
// it (all zeros: nothing pressed, LEDs off) when no message came for LinkTuning::TIMEOUT_MS. IsLinked() tells
// whether a message came in the last TIMEOUT_MS.
template <typename T>
class MessageReader {
public:
    void Receive(HardwareSerial &serial, T &message, uint32_t now) {
        if (Read(serial, message)) {
            lastMessageMs = now;
        }
        if (!IsLinked(now)) {
            message = {};
        }
    }

    bool IsLinked(uint32_t now) const {
        return now - lastMessageMs < LinkTuning::TIMEOUT_MS;
    }

private:
    bool Read(HardwareSerial &serial, T &message) {
        bool received = false;
        while (serial.available()) {
            const uint8_t byte = static_cast<uint8_t>(serial.read());
            if (count == 0) {
                if (byte == MessageLink::START_BYTE) {
                    count = 1;
                }
            } else if (count <= sizeof(T)) {
                buffer[count - 1] = byte;
                count++;
            } else {
                count = 0;
                if (MessageLink::Checksum(buffer, sizeof(T)) == byte) {
                    memcpy(&message, buffer, sizeof(T));
                    received = true;
                }
            }
        }
        return received;
    }

    uint8_t buffer[sizeof(T)] = {};
    size_t count = 0;
    uint32_t lastMessageMs = 0;
};
