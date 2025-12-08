#include "PiranhaMessage.h"

std::vector<uint8_t> PiranhaMessage::encodePacket() {
    buffer.clear();
    offset = 0;
    bitOffset = 0;

    encode();
    std::vector<uint8_t> header(7);
    header[0] = (id >> 8) & 0xFF;
    header[1] = id & 0xFF;
    
    size_t payloadSize = buffer.size();
    header[2] = (payloadSize >> 16) & 0xFF;
    header[3] = (payloadSize >> 8) & 0xFF;
    header[4] = payloadSize & 0xFF;
    
    header[5] = (version >> 8) & 0xFF;
    header[6] = version & 0xFF;
    
    std::vector<uint8_t> fullPacket;
    fullPacket.reserve(header.size() + payloadSize);
    fullPacket.insert(fullPacket.end(), header.begin(), header.end());
    fullPacket.insert(fullPacket.end(), buffer.begin(), buffer.end());
    
    return fullPacket;
}

void PiranhaMessage::ensureCapacity(size_t capacity) {
    if (offset + capacity > buffer.size()) {
        buffer.resize(offset + capacity);
    }
}

void PiranhaMessage::writeInt(int32_t value) {
    bitOffset = 0;
    ensureCapacity(4);
    buffer[offset++] = (value >> 24) & 0xFF;
    buffer[offset++] = (value >> 16) & 0xFF;
    buffer[offset++] = (value >> 8) & 0xFF;
    buffer[offset++] = value & 0xFF;
}

void PiranhaMessage::writeShort(int16_t value) {
    bitOffset = 0;
    ensureCapacity(2);
    buffer[offset++] = (value >> 8) & 0xFF;
    buffer[offset++] = value & 0xFF;
}

void PiranhaMessage::writeString(const std::string& value) {
    if (value.empty() || value.length() > 90000) {
        writeInt(-1);
        return;
    }
    writeInt(static_cast<int>(value.length()));
    for (char c : value) {
        ensureCapacity(1);
        buffer[offset++] = static_cast<uint8_t>(c);
    }
}

void PiranhaMessage::writeString() {
    writeInt(-1);
}

void PiranhaMessage::writeByte(uint8_t value) {
    bitOffset = 0;
    ensureCapacity(1);
    buffer[offset++] = value;
}

void PiranhaMessage::writeBoolean(bool value) {
    if (bitOffset == 0) {
        ensureCapacity(1);
        buffer[offset++] = 0;
    }
    if (value) {
        buffer[offset - 1] |= (1 << bitOffset);
    }
    bitOffset = (bitOffset + 1) & 7;
}

void PiranhaMessage::writeVInt(int32_t value) {
    bitOffset = 0;
    int32_t temp = (value >> 25) & 0x40;
    int32_t flipped = value ^ (value >> 31);
    temp |= value & 0x3F;
    value >>= 6;
    flipped >>= 6;
    
    if (flipped == 0) {
        writeByte(static_cast<uint8_t>(temp));
        return;
    }
    
    writeByte(static_cast<uint8_t>(temp | 0x80));
    flipped >>= 7;
    uint8_t r = (flipped != 0) ? 0x80 : 0;
    writeByte(static_cast<uint8_t>((value & 0x7F) | r));
    value >>= 7;
    
    while (flipped != 0) {
        flipped >>= 7;
        r = (flipped != 0) ? 0x80 : 0;
        writeByte(static_cast<uint8_t>((value & 0x7F) | r));
        value >>= 7;
    }
}

void PiranhaMessage::writeDataReference(int32_t csv, int32_t item) {
    if (csv != 0) {
        writeVInt(csv);
        writeVInt(item);
    } else if (item != 0) {
        writeVInt(0);
        writeVInt(item);
    } else if (csv == 0 && item == 0) {
        writeVInt(0);
    }
}

void PiranhaMessage::writeLongInt(int32_t value1, int32_t value2) {
    writeInt(value1);
    writeInt(value2);
}

void PiranhaMessage::writeLong(int64_t value) {
    writeInt(static_cast<int32_t>(value >> 32));
    writeInt(static_cast<int32_t>(value));
}

void PiranhaMessage::writeLongLong(int64_t value) {
    writeInt(static_cast<int32_t>(value >> 32));
    writeInt(static_cast<int32_t>(value));
}

void PiranhaMessage::writeBytes(const std::vector<uint8_t>& bytes) {
    if (!bytes.empty()) {
        writeInt(static_cast<int32_t>(bytes.size()));
        for (uint8_t byte : bytes) {
            writeByte(byte);
        }
    } else {
        writeInt(-1);
    }
}