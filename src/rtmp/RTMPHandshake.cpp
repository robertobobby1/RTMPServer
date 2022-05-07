
#include <unistd.h>
#include "rtmp/RTMPHandshake.h"
#include "Excepcions/SocketExcepcion.h"
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
    sendRandomBytes();
    sendHandshake();
    receiveBack();
}

/*
 * Will receive first message from client random bytes
 * it will safe those bytes to return them to client later
 */
void RTMPHandshake::receiveHandshake() const {
    int msgsize = 0;
    // read client's message
    msgsize = (int)read(clientFD, client_handshake, sizeof(client_handshake));
    check(msgsize, "error recieving client message\n");

    if (msgsize != HANDSHAKE_BUFFER_SIZE - 1) {
        throw SocketException("Not an rtmp packet");
    }
    // remove first byte (0x03) from clients request
    memmove((void*)client_handshake, (void*) (client_handshake + 1), sizeof(client_handshake) - 1 );
}

/*
 * Will generate random bytes to send to client and
 * receive those bytes back, if same bytes sended
 * conexion is completed
 */
void RTMPHandshake::sendRandomBytes() const {
    createHandshakeEcho();
    check(send(clientFD, randomBytes, sizeof(randomBytes), 0),
          "couldn't send correctly our 1537 randomBytes");
    // remove (0x03)(first element in array) from randomBytes
    memmove((void*)randomBytes, (void*) (randomBytes + 1), sizeof(randomBytes) - 1);
}

/*
 * Last step, send clients handshake back
 * first 4 bytes(clients time(unchanged)),
 * next 4 bytes our timestamp(initially set to 0)
 */
void RTMPHandshake::sendHandshake() const {
    client_handshake[5] = client_handshake[6] = client_handshake[7] = client_handshake[8] = 0;
    check(send(clientFD, client_handshake, sizeof(client_handshake), 0),
          "couldn't send client 1537 randomBytes back");
}

/*
 * 1 byte for version, 4 bytes for  time(initially we will set it to 0),
 * 4 bytes set to zero, the rest set to random bytes
 * RandomBytes for handshake
 */
void RTMPHandshake::createHandshakeEcho() {
    randomBytes[0] = (unsigned char)3;
    randomBytes[1] = randomBytes[2] = randomBytes[3] = randomBytes[4] =
    randomBytes[5] = randomBytes[6] = randomBytes[7] = randomBytes[8] = 0;
    // Fill random value.
    srand( 255 );
    //pBytes[1] = pBytes[2] = pBytes[3] = pBytes[4] =
    for( int i = 9; i < 1536; i++ )
    {
        randomBytes[i] = (unsigned char)rand();
    }
}

/*
 * receive our message back and compare it with our original buffer
 */
void RTMPHandshake::receiveBack() const {
    unsigned char client_handshakeBack[HANDSHAKE_BUFFER_SIZE];
    // reading 1536 random bytes back, no (0x03) sended back
    (int)read(clientFD, client_handshakeBack, HANDSHAKE_BUFFER_SIZE - 2);

    // Client didn't return handshake correctly
    if (memcmp(randomBytes, client_handshakeBack, HANDSHAKE_BUFFER_SIZE - 2) != 0){
        throw SocketException("Handshake was not completed correctly\n");
    }
}


void RTMPHandshake::check(int socket, const char *err) {
    if (socket == -1){
        perror(err);
        throw SocketException(err);
    }
}