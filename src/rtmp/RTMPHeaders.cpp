#include <iostream>
#include <bitset>
#include <cstring>
#include <netinet/in.h>
#include <cmath>
#include "rtmp/RTMPHeaders.h"
#include "rtmp/RTMPEnums.h"
#include "utils/Logger.h"
#include "Excepcions/RTMPHeaderExcepcion.h"

RTMPHeaders::RTMPHeaders(const char *full_buffer){
    initializeClassObjects(full_buffer);
}

/*
 * Processes basic header type and initializes those values on class variables
 */
void RTMPHeaders::BasicHeaderType(const char *full_buffer) {
    std::bitset<8> bits(full_buffer[0]);
    // basic header type bits from 2-7 stream id for 1byte length, 0 for 2byte length and 1 for 3byte length
    // remove two first bytes, (set to 0)
    bits.set(7, false); bits.set(6, false);
    unsigned char basic_header_identifier = (char)bits.to_ulong();
    // if ends with value 1 it'll be THREE_BYTE_HEADER, value 2 TWO_BYTE_HEADER otherwise stream id
    if (basic_header_identifier == '\001')
    {
        basic_header_type = BASIC_HEADER_TYPE::THREE_BYTE_HEADER;
        // rtmp stream id function for three byte header
        stream_id = (atoi(&full_buffer[1]) + 64) +
                    (256 * atoi(&full_buffer[2]));
        header_size = 3;
    }
    else if(basic_header_identifier == '\000')
    {
        basic_header_type = BASIC_HEADER_TYPE::TWO_BYTE_HEADER;
        // rtmp stream id function for two byte header
        stream_id = (atoi(&full_buffer[1]) + 64);
        header_size = 2;
    }
    else
    {
        basic_header_type = BASIC_HEADER_TYPE::ONE_BYTE_HEADER;
        stream_id = basic_header_identifier;
        header_size = 1;
    }
}

/*
 * Will read bytes, determine basic header and message header types,
 * will determine stream id and set rtmp header size as well as processing
 * message header
 */
void RTMPHeaders::initializeClassObjects(const char *full_buffer) {
    // get first byte in 8 bits
    std::bitset<8> bits(full_buffer[0]);
    BasicHeaderType(full_buffer);
    // Header message type 2 bits
    if (bits.test(7)){
        // (1,1) TYPE3
        if (bits.test(6)){
            message_header_type = MESSAGE_HEADER_TYPE::TYPE3;
            // empty
        }
        // (1,0) TYPE2
        else{
            message_header_type = MESSAGE_HEADER_TYPE::TYPE2;
            processType2MessageHeader(&full_buffer[header_size]);
        }
    }
    else{
        // (0,1) TYPE1
        if (bits.test(6)){
            message_header_type = MESSAGE_HEADER_TYPE::TYPE1;
            processType1MessageHeader(&full_buffer[header_size]);
        }
        // (0,0) TYPE0
        else{
            message_header_type = MESSAGE_HEADER_TYPE::TYPE0;
            processType0MessageHeader(&full_buffer[header_size]);
        }
    }

}

/*
 * TYPE 0 (11 bytes long)
 *   0                   1                   2                   3
 *   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                 timestamp                     |message length |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |      message length (cont)    |message type id| msg stream id |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |            message stream id (cont)           |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *  multi-byte objects need ntoh (network to host) little-endian to big-endian (in this case, see enianness of system)
 */
void RTMPHeaders::processType0MessageHeader(const char *buffer) {
    // 11 bytes long and a first 0x00 byte
    unsigned char data_buffer[11];
    header_size += 11;

    memcpy(data_buffer, buffer, 11);
    // three bytes timestamp shift bytes right to get only three
    timestamp = ntohl((*(unsigned int*) &data_buffer[0] << 8));
    // three bytes message length, get 4 bytes, first not wanted so remove first bytes w mask
    message_length = ntohl(*(unsigned int*) &data_buffer[2]) & 0x00ffffff;
    message_type_id = data_buffer[6];
    message_stream_id = ntohl(*(unsigned int*) &data_buffer[7]);
}

/*
 * TYPE 0 (7 bytes long)
 *   0                   1                   2                   3
 *   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                 timestamp                     |message length |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |      message length (cont)    |message type id|
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *  multi-byte objects need ntoh (network to host) little-endian to big-endian (in this case, see enianness of system)
 */
void RTMPHeaders::processType1MessageHeader(const char *buffer) {
    // 7 bytes long and a first 0x00 byte
    unsigned char data_buffer[7];
    header_size += 7;

    memcpy(data_buffer, buffer, 7);
    // three bytes timestamp shift bytes right to get only three
    timestamp = ntohl((*(unsigned int*) &data_buffer[0] << 8));
    // three bytes message length, get 4 bytes, first not wanted so remove first bytes w mask
    message_length = ntohl(*(unsigned int*) &data_buffer[2]) & 0x00ffffff;
    message_type_id = data_buffer[6];
}

/*
 * TYPE 2 (3 bytes long)
 *   0                   1                   2
 *   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |               timestamp delta                 |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *  multi-byte objects need ntoh (network to host) little-endian to big-endian (in this case, see enianness of system)
 */
void RTMPHeaders::processType2MessageHeader(const char *buffer) {
    // 3 bytes long and a first 0x00 byte
    unsigned char data_buffer[3];
    header_size += 3;

    memcpy(data_buffer, buffer, 3);
    // three bytes timestamp shift bytes right to get only three
    timestamp = ntohl((*(unsigned int*) &data_buffer[0] << 8));
}

/*
 * If message_type_id is in (1-6) it will be a control message
 * afterwards will have toi process control messages
 */
bool RTMPHeaders::isControlMessage() const {
    return message_type_id == 1 || message_type_id == 2 ||
           message_type_id == 3 || message_type_id == 4 ||
           message_type_id == 5 || message_type_id == 6;
}

/*
 * Builds a header and introduces it in a Buffer if any error
 * will return an empty Buffer
 */
Buffer RTMPHeaders::build_rtmp_header(BASIC_HEADER_TYPE basic_header, MESSAGE_HEADER_TYPE message_header, int cs_id,
                                      unsigned int timestamp, unsigned int msg_length, uint8_t msg_type_id, unsigned int msg_stream_id) {
    try{
        auto buff = build_basic_header(basic_header, cs_id);
        buff.append(build_message_header(message_header, timestamp, msg_length, msg_type_id, msg_stream_id));
        return buff;
    } catch (RTMPHeaderException &e ){
        Logger::log(Logger::VERBOSE_LOG, e.get_error_message());
        return {nullptr, 0};
    }
}

/*
 * Builds first three bytes based on
 * header type and chunk stream id
 */
Buffer RTMPHeaders::build_basic_header(BASIC_HEADER_TYPE basic_header, int cs_id) {
    auto buff = Buffer(nullptr, 0);
    switch (basic_header) {
        case BASIC_HEADER_TYPE::ONE_BYTE_HEADER: {
            if (cs_id > 63 || cs_id < 2) {
                throw RTMPHeaderException(&"Incompatible basic header and cs_id"[cs_id]);
            }
            auto temp = (const char *) &cs_id;
            buff.append(temp, 1);
            break;
        }
        case BASIC_HEADER_TYPE::TWO_BYTE_HEADER: {
            if (cs_id > 319 || cs_id < 64) {
                throw RTMPHeaderException(&"Incompatible basic header and cs_id"[cs_id]);
            }
            cs_id -= 64;
            unsigned char buffer[2];
            buffer[0] = 0; buffer[1] = (const char) cs_id;
            buff.append((const char*)&buffer[0], 2);
            break;
        }
        case BASIC_HEADER_TYPE::THREE_BYTE_HEADER: {
            if (cs_id > 65599 || cs_id < 64) {
                throw RTMPHeaderException(&"Incompatible basic header and cs_id"[cs_id]);
            }
            unsigned char buffer[3]; buffer[0] = '\001';
            if (cs_id > 63 && cs_id < 256){
                cs_id -= 64;
                buffer[1] = (const char) cs_id;
            } else {
                cs_id -= 64;
                buffer[1] = cs_id % 256;
                buffer[2] = (int) (cs_id / 256);
            }
            buff.append((const char*)&buffer[0], 3);
            break;
        }
    }
    return buff;
}

/*
 * Build message header part
 * type 0 has inside type 1 that has inside type 2, type 3 is empty
 */
Buffer RTMPHeaders::build_message_header(MESSAGE_HEADER_TYPE message_header, uint32_t timestamp,
                                         uint32_t msg_length, uint8_t msg_type_id, uint32_t msg_stream_id) {
    auto buff = Buffer(nullptr, 0);
    switch (message_header) {
        case MESSAGE_HEADER_TYPE::TYPE0: {
            if (timestamp >= 16777216) {
                throw RTMPHeaderException("Incompatible message header and null or too big timestamp");
            }
            auto _timestamp = htonl(timestamp);
            buff.append((const char*)&_timestamp, 3);
        }
        case MESSAGE_HEADER_TYPE::TYPE1: {
            if (msg_length >= 16777216) {
                throw RTMPHeaderException("Incompatible message header and null or too big message length");
            }
            if (msg_type_id == 0){
                throw RTMPHeaderException("Incompatible message header and null message type id");
            }
            // shift 8 bit left because we receive it in 4 byte data type
            auto _msg_length = htonl(msg_length)<<8;
            buff.append((const char*)&_msg_length, 3);

            buff.append((const char*)&msg_type_id, 1);
        }
        case MESSAGE_HEADER_TYPE::TYPE2:{
            auto _msg_stream_id = htonl(msg_stream_id);
            buff.append((const char*)&_msg_stream_id, 4);
        }
            break;
        case MESSAGE_HEADER_TYPE::TYPE3:
        default:
            break;
    }
    return buff;
}


