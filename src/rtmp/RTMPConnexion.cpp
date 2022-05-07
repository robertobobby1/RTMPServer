#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include "rtmp/RTMPConnexion.h"
#include "Excepcions/SocketExcepcion.h"
#include "utils/Logger.h"
#include "rtmp/RTMPHeaders.h"
#include "AMF0Decoder.h"
#include "rtmp/RTMPHandshake.h"
#include "utils/NotFoundExcepcion.h"

#define BUFSIZE 4096
#define HANDSHAKE_BUFFER_SIZE 1538


/*
 * This class will process this full initial RTMP connexion

+--------------+                +-------------+
|    Client    |      |         |    Server   |
+------+-------+      |         +------+------+
|              Handshaking done               |
|                     |                       |
|                     |                       |
|                     |                       |
|                     |                       |
|----------- Command Message(connect) ------->|
|                                             |
|<------- Window Acknowledgement Size --------|
|                                             |
|<----------- Set Peer Bandwidth -------------|
|                                             |
|-------- Window Acknowledgement Size ------->|
|                                             |
|<------ User Control Message(StreamBegin) ---|
|                                             |
|<------------ Command Message ---------------|
|       (_result- connect response)           |
 */

/*
 * Threads will enter this method to process RTMPHeaders request,
 * connection is already started, receive p_client which is
 * pointer to clients_socket file descriptor, in error close socket
 */
RTMPConnexion::RTMPConnexion(int p_client) : clientFD(p_client) {
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
void RTMPConnexion::processRequest() {
    // proccess handshake if any error exception will be thrown
    { (RTMPHandshake(clientFD)); }

    bool connexion_done = false;
    while (connexion_done){
        // connexion_done = true;

    }
    processNewPacket();
    close(clientFD);
}

/*
 * Receive data from clientFD and cut buffer to its size
 * to make a good usage of memory and return in struct of
 * msgsize and buffer pointer
 */
Buffer RTMPConnexion::receive() const {
    size_t bytes_read;
    const char* data_buffer = (const char*) malloc(BUFSIZE);
    // read bytes
    bytes_read = read(clientFD, (void *) data_buffer, BUFSIZE);

    // reallocate to free the rest of the waste memory
    data_buffer = (const char*)realloc((void *) data_buffer, bytes_read);

    auto buff = Buffer(data_buffer, (int)bytes_read);
    return buff;
}

/*
 * Will process one full buffer, this may contain more than one
 * rtmp packet so we iterate till every byte is processed
 */
void RTMPConnexion::processNewPacket() {
    Buffer full_buffer = receive();

    // Sometimes we receive more than one packet at the time, process all
    while(!full_buffer.is_end()){
        // process RTMPHeaders and cut buffer to data to process
        auto rtmp_headers = RTMPHeaders(full_buffer.get_actual_position());
        // move header size in buffer to process body
        full_buffer.move_buffer((int)rtmp_headers.header_size);

        processBody(&full_buffer, &rtmp_headers);
        // move body size in buffer
        full_buffer.move_buffer((int)rtmp_headers.message_length);
    }
}

/*
 * process body message depending on message type id body will be different
 */
void RTMPConnexion::processBody(Buffer* buff, RTMPHeaders* headers) {

    Logger::log(Logger::VERBOSE_LOG, &"Message type id" [ headers->message_type_id]);
    switch (headers->message_type_id){
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            // Control messages
            Logger::log(Logger::VERBOSE_LOG, "Control messages");
            return;
        case 8:
            // audio
            Logger::log(Logger::VERBOSE_LOG, "audio packet");
            return;
        case 9:
            // video
            Logger::log(Logger::VERBOSE_LOG, "video packet");
            return;
        case 15:
            // Data message AMF 3
            Logger::log(Logger::VERBOSE_LOG, "Data Message AMF 3");
            return;
        case 18:
            // Data message AMF 0
            Logger::log(Logger::VERBOSE_LOG, "Data Message AMF 0");
            return;
        case 16:
            // Shared Object Message AMF 3
            Logger::log(Logger::VERBOSE_LOG, "Data Message AMF 3");
            return;
        case 19:
            // Shared Object Message AMF 0
            Logger::log(Logger::VERBOSE_LOG, "Data Message AMF 0");
            return;
        case  17:
            // Command Message (AMF 3 encoded)
            Logger::log(Logger::VERBOSE_LOG, "Command Message AMF 3");
            return;
        case  20:
            // Command Message (AMF 0 encoded)
            Logger::log(Logger::VERBOSE_LOG, "Command Message AMF 0");
            processAMFCommand(buff, headers);
            return;
        case  22:
            return;
    }
}

/*
 * Process AMF command (should initially be connect command)
 */
void RTMPConnexion::processAMFCommand(Buffer* buff, RTMPHeaders* headers) {
    // decode packet
    AMFDataPacket packet;
    packet = AMF0Decoder::BlockDecoder(buff->get_actual_position(), (int)headers->message_length);
    // move byte errors of decoding
    buff->move_buffer(packet.byteError);

    packet.pprint();

    // get command
    std::string command = packet.getString("command");
    // if connect is not true, command should be connect
    if (!connect){
        if (command != "connect"){return;}

        // send window ack and set peer bandwidth
        connect  = true;
    }
}

/*
 * Send Buffer with window ACK size packet
 */
void RTMPConnexion::sendWindowACKSize() {
/*    Buffer buff = RTMPHeaders::build_rtmp_header();
    send(clientFD, buff.get_actual_position(), buff.get_size(), 0);*/
}
