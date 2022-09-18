#pragma once

#include <netinet/in.h>
#include "AMF0Encoder.h"
#include "AMF0Decoder.h"
#include "utils/NotFoundExcepcion.h"

# define htonll(x) (((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))

Buffer AMF0Encoder::BlockEncoder(AMFDataPacket packet) {
    // buffer to return
    auto buffer = Buffer(nullptr, 0);
    try {
        // command first
        std::string command = packet.getString("command");
        buffer.append(encodeString(command));

        // transaction id second
        double transaction_id = packet.getDouble("transaction_id");
        buffer.append(encodeDouble(transaction_id));

        // encode objects third
        for (const auto& j: packet.objectList){
            buffer.append(encodeObject(j));
        }

    } catch (NotFoundExcepcion &e){
        buffer.free_buffer();
        return {nullptr, 0};
    }
    return buffer;
}

Buffer AMF0Encoder::encodeObject(const AMFDataPacket& packet) {
    auto buffer = Buffer(nullptr, 0);
    unsigned char data_type = AMF_OBJECT;
    buffer.append((const char*)&data_type, 1);

    try {
        for (auto const& j : packet.Strings) {
            buffer.append(encodeString(j.first));
            buffer.append(encodeString(j.second));
        }
        for (auto const& j : packet.Doubles) {
            buffer.append(encodeString(j.first));
            buffer.append(encodeDouble(j.second));
        }
        for (auto const& j : packet.Booleans) {
            buffer.append(encodeString(j.first));
            buffer.append(encodeBoolean(j.second));
        }

        unsigned char end_array[3]; end_array[0] = 0x00; end_array[1] = 0x00; end_array[2] = AMF_END_OBJECT;
        buffer.append((const char*)&end_array, 3);
    } catch (NotFoundExcepcion &e) {
        buffer.free_buffer();
        return {nullptr, 0};
    }
    return buffer;
}

Buffer AMF0Encoder::encodeString(const std::string& string){
    auto buffer = Buffer(nullptr, 0);
    auto msgsize = htons((uint16_t) string.size());
    unsigned char data_type = AMF_STRING;

    buffer.append((const char *) &data_type, 1);
    buffer.append((const char *) &msgsize, 2);
    buffer.append(string.c_str(), msgsize);

    return buffer;
}

Buffer AMF0Encoder::encodeDouble(double number) {
    auto buffer = Buffer(nullptr, 0);
    unsigned char data_type = AMF_DOUBLE;
    // may give future problems
    auto inverted = htonll((uint64_t)number);

    buffer.append((const char *) &data_type, 1);
    buffer.append((const char*) &inverted, 8);

    return buffer;
}

Buffer AMF0Encoder::encodeBoolean(bool boolean){
    unsigned char byte;
    unsigned char data_type = AMF_BOOL;
    auto buffer = Buffer((const char*)&data_type, 1);

    if (boolean) {
        byte = 0x01;
    }else{
        byte = 0x00;
    }

    buffer.append((const char*)&byte, 1);
    return buffer;
}