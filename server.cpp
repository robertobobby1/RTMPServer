#include <unistd.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "Queue.h"
#include "server.h"
#include "RTMPRequestHandler.h"

#define SERVERPORT 1935
#define SOCKETERROR (-1)
#define SERVER_BACKLOG 100
#define THREAD_POOL_SIZE 30

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

// linux socket  identifiers are ints
int server_socket, client_socket, addr_size;
// structs for ip address and port
SA_IN server_addr;
// Queue to handle threads consuming and receiving requests
Queue threadsQueue;
// to initialize THREAD_POOL_SIZE threads and suspend till requests are sent
pthread_t thread_pool[THREAD_POOL_SIZE];
// mutex for Queue variable(threadsQueue) mutual exclusion
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// avoid active standby (taking CPU time for condition)
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;


int main(){

    printf("Starting process\n");
    start_socket();
    initialize_threads();
    printf("Start receiving connections\n");

    // loop on infinite incoming requests
    while(true){
        addr_size = sizeof(SA_IN);
        check(client_socket =
                accept(server_socket, (sockaddr*)&client_socket, (socklen_t*)&addr_size),
              "Couldn't accept external connection\n");
        printf("Connected!\n");

        int* p_client = (int*)malloc(sizeof(int));
        *p_client = client_socket;
        pthread_mutex_lock(&mutex);
        Queue::enqueue(p_client);
        // signal so that one thread dequeues
        pthread_cond_signal(&condition);
        pthread_mutex_unlock(&mutex);
    }
    return 0;
}

/*
 * Starts socket, binds it to file descriptor(linux)
 * and starts listening in SERVERPORT
 */
void start_socket(){
    // Create a socket
    check(server_socket = socket(AF_INET, SOCK_STREAM, 0),
    "Couldn't create socket");
    printf("socket created\n");

    // Set server address port and ip address in struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVERPORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the ip address and port to a socket
    check(bind(server_socket, (SA*)&server_addr, sizeof(server_addr)),
    "Couldn't bind to specified port and ip address");

    // strart listening on selected  port and ip address
    check(listen(server_socket, SERVER_BACKLOG),
    "Couldn't listen");
    printf("socket binded and listening \n");

}

void* handle_connection(void* arg) {
    int* p_client;
    while(true){
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&condition, &mutex);
        p_client = Queue::dequeue();
        pthread_mutex_unlock(&mutex);
        if (p_client != nullptr){
            RTMPRequestHandler::handleRequest(p_client);
        }
    }
}

/*
 * checks errors, if there is an error will output message
 * and exit if not continue execution
 */
void check(int socket, const char *err) {
    if (socket == SOCKETERROR){
        perror(err);
        exit(1);
    }
}

/*
 * Will initialize threads limited number of threads,
 * will then be suspended until requests arrive
 */
void initialize_threads() {
    for (unsigned long &i : thread_pool) {
        pthread_create(&thread_pool[i], nullptr, handle_connection, nullptr);
    }
}

