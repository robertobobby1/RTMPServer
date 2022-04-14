#pragma once
#ifndef RTMPSERVER_RTMPHANDSHAKE_H
#define RTMPSERVER_RTMPHANDSHAKE_H


class RTMPHandshake {

    public:
        explicit RTMPHandshake(int file_descriptor);

        void processHandshake() const;
        void receiveHandshake() const;
        void sendRandomBytesAndReceiveBack() const;
        void sendHandshake() const;
        static void check(int socket, const char *err);
        static void createHandshakeEcho();

        int clientFD;
};


#endif //RTMPSERVER_RTMPHANDSHAKE_H
