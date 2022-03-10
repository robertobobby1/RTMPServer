#include <iostream>
#include <bitset>
#include <cstring>
#include <netinet/in.h>
#include "RTMP.h"
#include "RTMPEnums.h"
#include "utils/TimestampManager.h"
#include "amf.hpp"

#define SWAP_INT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24));


RTMP::RTMP(const char *full_buffer, int msgsize){
    initializeClassObjects(full_buffer);
}

/*
 * Will read bytes, determine basic header and message header types,
 * will determine stream id and set rtmp header size as well as processing
 * message header
 */
void RTMP::initializeClassObjects(const char *full_buffer) {
    // get first byte in 8 bits
    std::bitset<8> bits(full_buffer[0]);
    BasicHeaderType(full_buffer);
    // Header message type 2 bits
    if (bits.test(7)){
        // (1,1) TYPE3
        if (bits.test(6)){
            message_header_type = MESSAGE_HEADER_TYPE::TYPE3;
            processType3MessageHeader(&full_buffer[header_size]);
        }
        // (1,0) TYPE2
        else{
            message_header_type = MESSAGE_HEADER_TYPE::TYPE2;
            processType2MessageHeader(&full_buffer[header_size]);
        }
    }
    else{
        // (0,1) TYPE3
        if (bits.test(6)){
            message_header_type = MESSAGE_HEADER_TYPE::TYPE1;
            processType1MessageHeader(&full_buffer[header_size]);
        }
        // (0,0) TYPE2
        else{
            message_header_type = MESSAGE_HEADER_TYPE::TYPE0;
            processType0MessageHeader(&full_buffer[header_size]);
        }
    }

}


/*
 * TYPE 0 (11 bytes long)
 *
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
void RTMP::processType0MessageHeader(const char *buffer) {
    // 11 bytes long and a first 0x00 byte
    unsigned char data_buffer[11];
    header_size += 11;

    memcpy(data_buffer, buffer, 11);
    data_buffer[0] = '\006';data_buffer[1] = '\000';data_buffer[2] = '\004';
    // three bytes timestamp shift bytes right to get only three
    timestamp = ntohl((*(unsigned int*) &data_buffer[0] << 8));
    // three bytes message length, get 4 bytes, first not wanted so remove first bytes w mask
    message_length = ntohl(*(unsigned int*) &data_buffer[2]) & 0x00ffffff;
    message_type_id = buffer[6];
    message_stream_id = ntohl(*(unsigned int*) &data_buffer[7]);
}

void RTMP::processType1MessageHeader(const char *buffer) {

}

void RTMP::processType2MessageHeader(const char *buffer) {

}

void RTMP::processType3MessageHeader(const char *buffer) {

}

void RTMP::BasicHeaderType(const char *full_buffer) {
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

















std::uint32_t RTMP::SwapBinary(std::uint32_t &value) {
    std::uint32_t tmp = ((value << 8) & 0xFF00FF00) | ((value >> 8) & 0xFF00FF);
    value = (tmp << 16) | (tmp >> 16);
}

void RTMP::printBits(unsigned char full_buffer[], int msgsize) {
    std::bitset<8> bits[4096];
    for (int i = 0; i < msgsize; i++) {
        bits[i] = std::bitset<8>(full_buffer[i]);
        std::cout << bits[i] << std::endl;
        i++;
    }
}
