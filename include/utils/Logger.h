#ifndef RTMPSERVER_LOGGER_H
#define RTMPSERVER_LOGGER_H

#include <string>

class Logger {
public:
    enum Error{
        SOCKET_LOG,
        SOCKET_ERROR_LOG,
        DATA_LOG,
        VERBOSE_LOG,
        BBDD_LOG
    };

    static void log(Error error_type, const char* err);
    static void write(const char *string, const char *err);
    static std::string currentDateToString();
};


#endif //RTMPSERVER_LOGGER_H
