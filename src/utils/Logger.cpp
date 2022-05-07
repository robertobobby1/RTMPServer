#include <cstdio>
#include "utils/Logger.h"
#include <iostream>
#include <fstream>
#include <cstring>

void Logger::log(Error error_type, const char* err) {
    switch (error_type) {
        case SOCKET_LOG:
            printf("%s", err);
            //Logger::write("logger/socket_log.txt" ,err);
            break;
        case SOCKET_ERROR_LOG:
            perror(err);
            break;
        default:
            printf("%s  ", err);
            break;
    }
}

void Logger::write(const char *file_path, const char *err) {
    std::ofstream log_file;
    log_file.open(file_path);
    log_file << currentDateToString();
    log_file << err;
    log_file << "\n";
    log_file.close();
}

std::string Logger::currentDateToString()
{
    time_t now = time(nullptr);
    tm *ltm = localtime(&now);

    std::string dateString, tmp;
    tmp = std::to_string(ltm->tm_mday);
    if (tmp.length() == 1)
        tmp.insert(0, "0");
    dateString += tmp;
    dateString += "-";
    tmp = std::to_string(1 + ltm->tm_mon);
    if (tmp.length() == 1)
        tmp.insert(0, "0");
    dateString += tmp;
    dateString += "-";
    tmp = std::to_string(1900 + ltm->tm_year);
    dateString += tmp;
    dateString += " ";
    tmp = std::to_string(ltm->tm_hour);
    if (tmp.length() == 1)
        tmp.insert(0, "0");
    dateString += tmp;
    dateString += ":";
    tmp = std::to_string(1 + ltm->tm_min);
    if (tmp.length() == 1)
        tmp.insert(0, "0");
    dateString += tmp;
    dateString += ":";
    tmp = std::to_string(1 + ltm->tm_sec);
    if (tmp.length() == 1)
        tmp.insert(0, "0");
    dateString += tmp;

    return dateString + " ";
}