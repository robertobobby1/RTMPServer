#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include "RTMPRequestHandler.h"
#include "RandomByteGenerator.h"
#include "SocketExcepcion.h"
#include "Logger.h"

#define BUFSIZE 4096
#define HANDSHAKE_BUFFER_SIZE 1538

/*
 * Threads will enter this method to process RTMP request,
 * connection is already started, receive p_client which is
 * pointer to clients_socket file descriptor, in error close socket
 */
void RTMPRequestHandler::handleRequest(const int* p_client) {
    int clientFD = *p_client;
    try {
        processRequest(clientFD);
    }catch (const SocketException &e) {
        close(clientFD);
        Logger::log(Logger::SOCKET_ERROR_LOG, e.get_error_message());
    }
}

/*
 * Main request processor
 */
void RTMPRequestHandler::processRequest(int clientFD){
    receiveAndSendHandshake(clientFD);
    sendRandomBytesAndReceiveBack(clientFD);
    close(clientFD);
}

void RTMPRequestHandler::check(int socket, const char *err) {
    if (socket == -1){
        perror(err);
        throw SocketException(err);
    }
}

/*
 * Will receive first message from client random bytes
 * it will safe those bytes to return them to client later
 */
void RTMPRequestHandler::receiveAndSendHandshake(int clientFD){
    unsigned char client_handshake[HANDSHAKE_BUFFER_SIZE];
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
        throw SocketException("Not an rtmp packet");
    }
    // null terminate buffer (keep only clients sent data)
    client_handshake[msgsize-1] = 0;
    // remove first byte (0x03) from clients request
    *client_handshake = (*client_handshake + 1);
    // Send client handshake back
    check(send(clientFD, client_handshake, sizeof(client_handshake), 0),
          "couldn't send client 1537 randomBytes back");
}


/*
 * Will generate random bytes to send to client and
 * receive those bytes back, if same bytes sended
 * conexion is completed
 */
void RTMPRequestHandler::sendRandomBytesAndReceiveBack(int clientFD){
    unsigned char randomBytes[HANDSHAKE_BUFFER_SIZE];

    memmove(randomBytes,RandomByteGenerator::run(), sizeof(randomBytes));
    check(send(clientFD, randomBytes, sizeof(randomBytes), 0),
        "couldn't send correctly our 1537 randomBytes");
    // remove (0x03)(first element in array) from randomBytes
    *randomBytes = (*randomBytes + 1);
    unsigned char client_handshakeBack[HANDSHAKE_BUFFER_SIZE];
    // to receive bytes_readed from fileDescriptor(socket)
    size_t bytes_read;
    // will keep message size
    int msgsize = 0;
    while(msgsize < HANDSHAKE_BUFFER_SIZE - 1){
        bytes_read = read(clientFD, client_handshakeBack+msgsize, sizeof(client_handshakeBack));
        msgsize += (int)bytes_read;
    }
    // Client didn't return handshake correctly
    if (memcmp(randomBytes, client_handshakeBack, HANDSHAKE_BUFFER_SIZE) != 0){
        throw SocketException("Handshake was not completed correctly\n");
    }

}

void RTMPRequestHandler::receive(int clientFD){
    int msgsize = 0; size_t bytes_read;
    unsigned char dataBuffer[BUFSIZE];

    while(msgsize < BUFSIZE){
        bytes_read = read(clientFD, dataBuffer+msgsize, sizeof(dataBuffer));
        msgsize += (int)bytes_read;
    }
}