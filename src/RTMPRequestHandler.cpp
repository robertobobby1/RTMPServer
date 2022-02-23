#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include "RTMPRequestHandler.h"
#include "RandomByteGenerator.h"


#define BUFSIZE 4096
#define TIMEOUT_SECONDS 10

struct timeval timeout;
char client_handshake[1536];


void RTMPRequestHandler::handleRequest(const int* p_client) {

    int clientFD = *p_client;
    receiveHandshake(clientFD);
    sendHandshakeBack(clientFD);
    // output on command line full request
    printf("REQUEST: %s\n", client_handshake);
    fflush(stdout);

    // if flag sent terminate
    close(clientFD);
}

void RTMPRequestHandler::check(int socket, const char *err) {
    if (socket == -1){
        perror(err);
        exit(1);
    }
}

void RTMPRequestHandler::receiveHandshake(int clientFD){
    char buffer[BUFSIZE];
    // to receive bytes_readed from fileDescriptor(socket)
    size_t bytes_read;
    // will keep message size
    int msgsize = 0;

    // read client's message
    while(msgsize < 1537){
        bytes_read = read(clientFD, buffer+msgsize, sizeof(buffer)-msgsize-1);
        msgsize += (int)bytes_read;
        if (msgsize > BUFSIZE-1 || buffer[msgsize-1] == '\n') {
            break;
        }
    }
    check((int)bytes_read, "error recieving client message\n");

    if (msgsize-1 != 1536) {
        printf("Not an rtmp packet");
        return;
    }
    // null terminate buffer (keep only clients sent data)
    buffer[msgsize-1] = 0;
    *client_handshake = *buffer;
}

void RTMPRequestHandler::sendHandshakeBack(int clientFD) {
    RandomByteGenerator::run();
    //send(clientFD, );
}
