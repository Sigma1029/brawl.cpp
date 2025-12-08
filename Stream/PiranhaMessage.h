#ifndef PIRANHAMESSAGE_H
#define PIRANHAMESSAGE_H

#include <vector>
#include <cstdint>
#include <string>

class PiranhaMessage {
public:
    std::vector<uint8_t> buffer;
    size_t offset = 0;
    size_t bitOffset = 0;
    uint16_t id = 0;
    uint16_t version = 0;
    
    virtual ~PiranhaMessage() = default;
    
    virtual void encode() = 0;
    std::vector<uint8_t> encodePacket();
    
    void ensureCapacity(size_t capacity);
    
    void writeInt(int32_t value);
    void writeShort(int16_t value);
    void writeString(const std::string& value);
    void writeString();
    void writeByte(uint8_t value);
    void writeBoolean(bool value);
    void writeVInt(int32_t value);
    void writeDataReference(int32_t csv, int32_t item);
    void writeLongInt(int32_t value1, int32_t value2);
    void writeLong(int64_t value);
    void writeLongLong(int64_t value);
    void writeBytes(const std::vector<uint8_t>& bytes);
};

#endif