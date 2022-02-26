#ifndef RTMPSERVER_LOGGER_H
#define RTMPSERVER_LOGGER_H


class Logger {
public:
    enum Error{
        SOCKET_LOG,
        SOCKET_ERROR_LOG,
        DATA_LOG,
        BBDD_LOG
    };

    static void log(Error error_type, const char* err);
};


#endif //RTMPSERVER_LOGGER_H
