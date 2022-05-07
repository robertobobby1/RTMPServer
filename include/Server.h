#pragma once

#define RTMPSERVER_SERVER_H


class Server {
    public:
        Server(int port, int backlog);

        bool initializeServer();
        void initializeServerHandler();
        [[nodiscard]] int acceptConnection() const;

        static void check(int socket, const char *err);

        int _server_fd = -1;
        int _port;
        int _backlog;

};
