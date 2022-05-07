#include "rtmp/RTMPServer.h"
#include "rtmp/RTMPConnexion.h"
#include "utils/Logger.h"
#include "Server.h"

static int getThreadSafeClientFD();
static void setThreadSafeClientFD(int _client_fd);
// mutex for Queue variable(threadsQueue) mutual exclusion
std::mutex mutex;
// avoid active standby (taking CPU time for condition)
std::condition_variable condition;
// data structure where threads will consume
std::queue<int> *queue;

RTMPServer::RTMPServer() = default;

[[noreturn]]
void RTMPServer::run(){
    Logger::log(Logger::SOCKET_LOG, "Starting process\n");

    Server _server(SERVERPORT, SERVER_BACKLOG);
    // try to start socket, if error, retry
    while (!_server.initializeServer()){
        sleep(10);
    }

    ThreadInitializer();
    Logger::log(Logger::SOCKET_LOG, "Start receiving connections\n");

    // initialize queue structure for threads to consume on new requests
    queue = new std::queue<int>();
    // loop on infinite incoming requests
    while(true) {
        int _client_fd = _server.acceptConnection();
        setThreadSafeClientFD(_client_fd);
        condition.notify_one();
    }
}

/*
 * Infinite loop for threads to process new requests
 * will be suspended when accessing client fd if not available still
 */
[[noreturn]]
static void *staticThreadInitializer(void* arg) {
    int p_client;
    while(true){
        // get file descriptor from queue filled by main
        p_client = getThreadSafeClientFD();
        if (p_client != -1){
            { (RTMPConnexion(p_client)); }
        }
    }
}

/*
 * Consumer method(only accesed by threads) thread safe
 * should be used by awaking via condition one thread to avoid active wait
 */
static int getThreadSafeClientFD() {
    int p_client;
    // lock and wait mutex
    std::unique_lock<std::mutex> ul(mutex);
    condition.wait(ul);
    if (queue->empty())
        return -1;
    p_client = queue->front();
    queue->pop();
    return p_client;
}

/*
 * Will initialize threads, limited number of threads,
 * will then be suspended until requests arrive
 */
void RTMPServer::ThreadInitializer() {
    for (unsigned long &i : connection_threads) {
        pthread_create(&connection_threads[i], nullptr,
                       staticThreadInitializer, nullptr);
    }
}

/*
 * Adds to queue clients file descriptor
 * Thread safe only accesed by "main" thread
 */
static void setThreadSafeClientFD(int _client_fd) {
    // when lock_guard destructor is called (end scope) mutex is relesed
    std::lock_guard<std::mutex> lg(mutex);
    queue->push(_client_fd);
}