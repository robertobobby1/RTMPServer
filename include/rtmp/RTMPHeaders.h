#pragma once

#ifndef RTMPSERVER_RTMPHEADERS_H
#define RTMPSERVER_RTMPHEADERS_H

#include "RTMPEnums.h"
#include "utils/Buffer.h"

class RTMPHeaders {
    public:

        explicit RTMPHeaders(const char *full_buffer);
        void initializeClassObjects(const char *buffer);

        void BasicHeaderType(const char *buffer);
        void processType0MessageHeader(const char *buffer);
        void processType1MessageHeader(const char *buffer);
        void processType2MessageHeader(const char *buffer);

        static Buffer build_rtmp_header(BASIC_HEADER_TYPE, MESSAGE_HEADER_TYPE, int cs_id, unsigned int timestamp = 16777216,
                                        unsigned int msg_length = 16777216, uint8_t msg_type_id = 0, unsigned int msg_stream_id = 0);
        static Buffer build_message_header(MESSAGE_HEADER_TYPE, unsigned int timestamp = 16777216, unsigned int msg_length = 16777216,
                                           uint8_t msg_type_id = 0, unsigned int msg_stream_id = 0);
        static Buffer build_basic_header(BASIC_HEADER_TYPE, int cs_id);
        [[nodiscard]] bool isControlMessage() const;

        unsigned int header_size = 0;
        int stream_id, message_type_id;
        unsigned int message_stream_id, timestamp, message_length;
        BASIC_HEADER_TYPE basic_header_type;
        MESSAGE_HEADER_TYPE message_header_type;

};


#endif //RTMPSERVER_RTMPHEADERS_H
