#pragma once

#ifndef RTMPSERVER_RTMP_H
#define RTMPSERVER_RTMP_H

#include "RTMPEnums.h"

class RTMP {
    public:

        explicit RTMP(const char *full_buffer);
        void initializeClassObjects(const char *buffer);

        void BasicHeaderType(const char *buffer);
        void processType0MessageHeader(const char *buffer);
        void processType1MessageHeader(const char *buffer);
        void processType2MessageHeader(const char *buffer);
        void processType3MessageHeader(const char *buffer);

        bool isControlMessage() const;

        unsigned int header_size = 0;
        int stream_id, message_type_id;
        unsigned int message_stream_id, timestamp, message_length;
        BASIC_HEADER_TYPE basic_header_type;
        MESSAGE_HEADER_TYPE message_header_type;


};


#endif //RTMPSERVER_RTMP_H
