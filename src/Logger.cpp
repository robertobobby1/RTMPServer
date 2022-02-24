#include <cstdio>
#include "Logger.h"

void Logger::log(Error error_type, const char* err) {
    perror(err);
}
