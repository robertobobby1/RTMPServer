#ifndef RTMPSERVER_AMF0DECODER_H
#define RTMPSERVER_AMF0DECODER_H

#include <string>
#include <map>
#include <list>
#include "AMFDataPacket.h"

struct AMFObject{
    std::unordered_map<std::string, bool> Booleans;
    std::unordered_map<std::string, std::string> Strings;
    std::unordered_map<std::string, double> Doubles;
};

enum amf_types : char {
    AMF_DOUBLE = '\000',
    AMF_BOOL = '\001',
    AMF_STRING = '\002',
    AMF_OBJECT = '\003',
    AMF_NULL = '\005',
    AMF_END_OBJECT = '\011',
};

class AMF0Decoder {
    public:
        static AMFDataPacket BlockDecoder(const char *buffer, int length);
        void UniTypeDecoder(const char *buffer);

        AMF0Decoder(){processed = 0;}

        std::string processString(const char*);
        AMFDataPacket processObject(const char*);
        double processDouble(const char*);
        bool processBoolean(const char*);
        bool isEndObject(const char*) const;

        int processed;
};


#endif //RTMPSERVER_AMF0DECODER_H
