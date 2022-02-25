#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include "RTMPRequestHandler.h"
#include "RandomByteGenerator.h"
#include "LoggerException.h"
#include "Logger.h"

#define BUFSIZE 4096
#define HANDSHAKE_BUFFER_SIZE 1538

unsigned char client_handshake[HANDSHAKE_BUFFER_SIZE];
unsigned char client_handshakeBack[HANDSHAKE_BUFFER_SIZE];
unsigned char randomBytes[HANDSHAKE_BUFFER_SIZE];

/*
 * Threads will enter this method to process RTMP request,
 * connection is already started, receive p_client which is
 * pointer to clients_socket file descriptor, in error close socket
 */
void RTMPRequestHandler::handleRequest(const int* p_client) {
    int clientFD = *p_client;
    try {
        processRequest(clientFD);
    }catch (const LoggerException &e) {
        close(clientFD);
        Logger::log(Logger::SOCKET_LOG, e.get_error_message());
    }
}

/*
 * Main request processor
 */
void RTMPRequestHandler::processRequest(int clientFD){
    receiveHandshake(clientFD);
    sendRandomBytes(clientFD);
    sendHandshakeBack(clientFD);
    receiveHandshakeBack(clientFD);
    // output on command line full request
    printf("REQUEST: %s\n", client_handshake);
    fflush(stdout);

    // if flag sent terminate
    close(clientFD);
}

void RTMPRequestHandler::check(int socket, const char *err) {
    if (socket == -1){
        perror(err);
        throw LoggerException(err);
    }
}

/*
 * Will receive first message from client random bytes
 * it will safe those bytes to return them to client later
 */
void RTMPRequestHandler::receiveHandshake(int clientFD){

    // to receive bytes_readed from fileDescriptor(socket)
    size_t bytes_read;
    // will keep message size
    int msgsize = 0;
    // read client's message
    while(msgsize < HANDSHAKE_BUFFER_SIZE - 1){
        bytes_read = read(clientFD, client_handshake+msgsize, sizeof(client_handshake));
        msgsize += (int)bytes_read;
    }
    check((int)bytes_read, "error recieving client message\n");

    if (msgsize != HANDSHAKE_BUFFER_SIZE - 1) {
        printf("Not an rtmp packet");
        return;
    }
    // null terminate buffer (keep only clients sent data)
    client_handshake[msgsize-1] = 0;
}

/*
 * will send clients random bytes back removing first byte (0x03)
 */
void RTMPRequestHandler::sendHandshakeBack(int clientFD) {
    // remove first byte (0x03) from clients request
    *client_handshake = (*client_handshake + 1);
    check(send(clientFD, client_handshake, sizeof(client_handshake), 0),
          "couldn't send client 1537 randomBytes back");
}

/*
 * Will generate random bytes to send to client
 */
void RTMPRequestHandler::sendRandomBytes(int clientFD){
    memmove(randomBytes,RandomByteGenerator::run(), sizeof(randomBytes));
    check(send(clientFD, randomBytes, sizeof(client_handshake), 0),
        "couldn't send correctly our 1537 randomBytes");
}

/*
 *
 */
void RTMPRequestHandler::receiveHandshakeBack(int clientFD){
    // to receive bytes_readed from fileDescriptor(socket)
    size_t bytes_read;
    // will keep message size
    int msgsize = 0;
    while(msgsize < HANDSHAKE_BUFFER_SIZE){
        bytes_read = read(clientFD, client_handshakeBack+msgsize, sizeof(client_handshakeBack));
        msgsize += (int)bytes_read;
    }
    int x = 0;
    msgsize = 0;
    while(msgsize < HANDSHAKE_BUFFER_SIZE){
        bytes_read = read(clientFD, client_handshakeBack+msgsize, sizeof(client_handshakeBack));
        msgsize += (int)bytes_read;
    }

}