#ifndef RTMPSERVER_TIMESTAMPMANAGER_H
#define RTMPSERVER_TIMESTAMPMANAGER_H


#include <cstdint>
#include "Bytes.h"


class TimestampManager {
    public:
        static Bytes getCurrentTime();
};


#endif //RTMPSERVER_TIMESTAMPMANAGER_H
