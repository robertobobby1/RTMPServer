#ifndef RTMPSERVER_RTMPREQUESTHANDLER_H
#define RTMPSERVER_RTMPREQUESTHANDLER_H

#include <bitset>

struct buffer_message{
    int msgsize;
    unsigned char* buffer;
};

class RTMPRequestHandler {

    public:

        explicit RTMPRequestHandler(const int* clientFD);
        void handleRequest() const;
        static void check(int socket, const char *err);
        void processRequest() const;
        void receive1() const;
        buffer_message receive() const;

        int clientFD;
};


#endif //RTMPSERVER_RTMPREQUESTHANDLER_H
