#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "RTMPRequestHandler.h"
#include "utils/SocketExcepcion.h"
#include "utils/Logger.h"
#include "RTMP.h"
#include "AMF0Decoder.h"
#include "RTMPHandshake.h"
#include "utils/NotFoundExcepcion.h"

#define BUFSIZE 4096
#define HANDSHAKE_BUFFER_SIZE 1538

RTMPRequestHandler::RTMPRequestHandler(const int* p_client) : clientFD(*p_client) {
    handleRequest();
}

/*
 * Threads will enter this method to process RTMP request,
 * connection is already started, receive p_client which is
 * pointer to clients_socket file descriptor, in error close socket
 */
void RTMPRequestHandler::handleRequest() const {
    try {
        processRequest();
    }catch (const SocketException &e) {
        close(clientFD);
        Logger::log(Logger::SOCKET_ERROR_LOG, e.get_error_message());
    }
}

/*
 * Main request processor
 */
void RTMPRequestHandler::processRequest() const {
    { (RTMPHandshake(clientFD)); }
    while(true){
        receive1();
    }
    close(clientFD);
}

/*
 *
 */
void RTMPRequestHandler::receive1() const {
    int processed = 0;
    buffer_message data_buffer = receive();

    while(processed < data_buffer.msgsize){
        // process RTMP headers and cut buffer to data to process
        RTMP rtmp = RTMP((const char*)&data_buffer.buffer[processed]);
        unsigned char amf_buffer[rtmp.message_length];
        memcpy(amf_buffer, &data_buffer.buffer[rtmp.header_size+processed], rtmp.message_length);

        processed += (int)(rtmp.message_length + rtmp.header_size);
        // control message not AMF encoded
        if (rtmp.isControlMessage())
            continue;

        // process message_length (AMF encoded)
        AMFDataPacket packet;
        packet = AMF0Decoder::BlockDecoder((const char*)amf_buffer, (int)rtmp.message_length);
        packet.pprint();
    }
}

void RTMPRequestHandler::check(int socket, const char *err) {
    if (socket == -1){
        perror(err);
        throw SocketException(err);
    }
}

/*
 * Receive data from clientFD and cut buffer to its size
 * to make a good usage of memory and return in struct of
 * msgsize and buffer pointer
 */
buffer_message RTMPRequestHandler::receive() const {
    buffer_message buff{};
    size_t bytes_read;
    unsigned char data_buffer[1024];
    // read bytes
    bytes_read = read(clientFD, data_buffer, BUFSIZE);
    buff.msgsize = (int)bytes_read;
    buff.buffer = (unsigned char*) malloc(buff.msgsize);
    memcpy(buff.buffer, data_buffer, buff.msgsize);

    return buff;
}



