#pragma once

#include <unistd.h>
#include "RTMPHandshake.h"
#include "utils/SocketExcepcion.h"
#include <cstdio>
#include <cstring>
#include <sys/socket.h>

#define HANDSHAKE_BUFFER_SIZE 1538

unsigned char client_handshake[HANDSHAKE_BUFFER_SIZE];
unsigned char randomBytes[HANDSHAKE_BUFFER_SIZE];

/*
 * Will process the handshake, excepcions not caught
 * should be caught where instantiated RTMPHandshake,
 * without handshake cannot continue request
 */
RTMPHandshake::RTMPHandshake(int file_descriptor) : clientFD(file_descriptor) {
    processHandshake();
}

/*
 * Full handshake processor
 */
void RTMPHandshake::processHandshake() const {
    receiveHandshake();
    sendRandomBytesAndReceiveBack();
    sendHandshake();
}

/*
 * Will receive first message from client random bytes
 * it will safe those bytes to return them to client later
 */
void RTMPHandshake::receiveHandshake() const {
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
 * Will generate random bytes to send to client and
 * receive those bytes back, if same bytes sended
 * conexion is completed
 */
void RTMPHandshake::sendRandomBytesAndReceiveBack() const {
    unsigned char client_handshakeBack[HANDSHAKE_BUFFER_SIZE];
    // to receive bytes_readed from fileDescriptor(socket)
    size_t bytes_read;
    // will keep message size
    int msgsize = 0;
    createHandshakeEcho();

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

/*
 * Last step, send clients handshake back
 */
void RTMPHandshake::sendHandshake() const {
    check(send(clientFD, client_handshake, sizeof(client_handshake), 0),
          "couldn't send client 1537 randomBytes back");
}

/*
 * RandomBytes for handshake
 */
void RTMPHandshake::createHandshakeEcho() {
    // Fill random value.
    srand( 255 );
    // first byte (0x03)
    randomBytes[0] = (unsigned char)3;
    //pBytes[1] = pBytes[2] = pBytes[3] = pBytes[4] =
    for( int i = 1; i < 1536; i++ )
    {
        randomBytes[i] = (unsigned char)rand();
    }
}

void RTMPHandshake::check(int socket, const char *err) {
    if (socket == -1){
        perror(err);
        throw SocketException(err);
    }
}

