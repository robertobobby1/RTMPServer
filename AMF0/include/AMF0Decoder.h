#ifndef RTMPSERVER_AMF0DECODER_H
#define RTMPSERVER_AMF0DECODER_H

#include <string>
#include <map>
#include <list>

#define MAX_OBJECT_SIZE 5
#define MAX_TOTAL_OBJECTS 3

struct PairValue{
    std::string key;
    std::string value;
};

struct Object{
    PairValue object[MAX_OBJECT_SIZE];
};

struct AMFPacket{
    std::string command;
    double transaction_id;
    Object objects[MAX_TOTAL_OBJECTS];
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
        static AMFPacket BlockDecoder(const char *buffer, int length);
        void UniTypeDecoder(const char *buffer);

        AMF0Decoder(){processed = 0;}

        std::string processString(const char*);
        Object processObject(const char*);
        double processDouble(const char*);
        bool processBoolean(const char*);
        bool isEndObject(const char*) const;

        int processed;
};


#endif //RTMPSERVER_AMF0DECODER_H
