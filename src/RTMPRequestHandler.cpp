#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include "RTMPRequestHandler.h"
#include "utils/RandomByteGenerator.h"
#include "utils/SocketExcepcion.h"
#include "utils/Logger.h"
#include "deserializer.hpp"
#include "amfbool.hpp"
#include "RTMP.h"
#include "amfobject.hpp"
#include "amfstring.hpp"
#include "AMF0Decoder.h"


#define BUFSIZE 4096
#define HANDSHAKE_BUFFER_SIZE 1538

unsigned char client_handshake[HANDSHAKE_BUFFER_SIZE];

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
    receiveHandshake(clientFD);
    sendRandomBytesAndReceiveBack(clientFD);
    sendHandshake(clientFD);
    receive(clientFD);
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
        throw SocketException("Not an rtmp packet");
    }
    // remove first byte (0x03) from clients request
    memmove((void*)client_handshake, (void*) (client_handshake + 1), sizeof(client_handshake) - 1);
}

/*
 * Last step, send clients handshake back
 */
void RTMPRequestHandler::sendHandshake(int clientFD){
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
    unsigned char client_handshakeBack[HANDSHAKE_BUFFER_SIZE];
    // to receive bytes_readed from fileDescriptor(socket)
    size_t bytes_read;
    // will keep message size
    int msgsize = 0;

    memmove(randomBytes,RandomByteGenerator::createHandshakeEcho(), sizeof(randomBytes));

    check(send(clientFD, randomBytes, sizeof(randomBytes), 0),
        "couldn't send correctly our 1537 randomBytes");
    // remove (0x03)(first element in array) from randomBytes
    memmove((void*)randomBytes, (void*) (randomBytes + 1), sizeof(randomBytes) - 1);

    // reading 1536 random bytes back, no (0x03) sended back
    while(msgsize < HANDSHAKE_BUFFER_SIZE - 2){
        bytes_read = read(clientFD, client_handshakeBack+msgsize, HANDSHAKE_BUFFER_SIZE - 2);
        msgsize += (int)bytes_read;
    }
    // Client didn't return handshake correctly
    if (memcmp(randomBytes, client_handshakeBack, HANDSHAKE_BUFFER_SIZE - 2) != 0){
        throw SocketException("Handshake was not completed correctly\n");
    }
}

void RTMPRequestHandler::receive(int clientFD){
    int msgsize = 0; size_t bytes_read;
    unsigned char data_buffer[1024];
    // read bytes
    bytes_read = read(clientFD, data_buffer, BUFSIZE);
    msgsize += (int)bytes_read;
    int processed = 0;

    while(processed < msgsize){
        RTMP rtmp = RTMP((const char*)&data_buffer[processed]);
        unsigned char amf_buffer[rtmp.message_length];
        memcpy(amf_buffer, &data_buffer[rtmp.header_size+processed], rtmp.message_length);

        AMFPacket packet;
        packet = AMF0Decoder::BlockDecoder((const char*)amf_buffer, msgsize);
        processed += (int)(rtmp.message_length + rtmp.header_size);
    }
}

