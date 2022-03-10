#ifndef RTMPSERVER_RTMPREQUESTHANDLER_H
#define RTMPSERVER_RTMPREQUESTHANDLER_H

#include <bitset>

class RTMPRequestHandler {

    public:
        static void handleRequest(const int* p_client);
        static void check(int socket, const char *err);
        static void receiveHandshake(int clientFD);
        static void sendRandomBytesAndReceiveBack(int clientFD);
        static void processRequest(int clientFD);
        static void sendHandshake(int clientFD);
        static void receive(int clientFD);


};


#endif //RTMPSERVER_RTMPREQUESTHANDLER_H
