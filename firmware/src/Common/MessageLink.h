#pragma once
#include <Arduino.h>
#include <cstring>

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

// Reads messages of type T without blocking: Read() returns true when a whole, valid message has arrived.
template <typename T>
class MessageReader {
public:
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

private:
    uint8_t buffer[sizeof(T)] = {};
    size_t count = 0;
};
