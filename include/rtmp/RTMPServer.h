#pragma once
#define SERVERPORT 1935
#define SERVER_BACKLOG 100
#define THREAD_POOL_SIZE 5

#include <netinet/in.h>
#include <queue>
#include <unistd.h>
#include <mutex>
#include <condition_variable>

class RTMPServer {
    public:

        RTMPServer();

        [[noreturn]] void run();
        void ThreadInitializer();

        // to initialize THREAD_POOL_SIZE threads and suspend till requests are sent
        pthread_t connection_threads[THREAD_POOL_SIZE]{};
};

