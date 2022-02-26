#include <cstdio>
#include "Logger.h"

void Logger::log(Error error_type, const char* err) {
    switch (error_type) {
        case SOCKET_LOG:
            printf("%s", err);
            break;
        case SOCKET_ERROR_LOG:
            perror(err);
            break;
        case DATA_LOG:
            printf("%s ", err);
            break;
        case BBDD_LOG:
            printf("%s  ", err);
            break;
    }
}
