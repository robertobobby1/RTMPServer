#ifndef RTMPSERVER_RTMPREQUESTHANDLER_H
#define RTMPSERVER_RTMPREQUESTHANDLER_H


class RTMPRequestHandler {

    public:
        static void handleRequest(const int* p_client);
        static void check(int socket, const char *err);
        static void receiveHandshake(int clientFD);
        static void sendHandshakeBack(int clientFD);
};


#endif //RTMPSERVER_RTMPREQUESTHANDLER_H
