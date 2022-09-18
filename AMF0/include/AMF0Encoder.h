#pragma once
#ifndef RTMPSERVER_AMF0ENCODER_H
#define RTMPSERVER_AMF0ENCODER_H


#include "AMFDataPacket.h"
#include "../../include/utils/Buffer.h"

class AMF0Encoder {
    public:
        static Buffer BlockEncoder(AMFDataPacket packet);
        static Buffer encodeObject(const AMFDataPacket& packet);
        static Buffer encodeString(const std::string& string);
        static Buffer encodeDouble(double string);
        static Buffer encodeBoolean(bool boolean);
};


#endif //RTMPSERVER_AMF0ENCODER_H
