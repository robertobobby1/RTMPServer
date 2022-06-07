#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <memory>
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

    receiveAndProcessConnect();
    // if connect packet is empty no connect received retry once
    if (connect_packet.is_empty()){
        receiveAndProcessConnect();
        if (connect_packet.is_empty()){
            throw SocketException("Couldn't receive connect from client, closing connection");
        }
    }
    // send window ack and set peer bandwidth
    if (sendWindowACKSize() == -1 && sendSetPeerBandwidth() == -1){
        throw SocketException("Error sending window ACK or Peer bandwidth");
    }

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

    data_buffer = (const char*) realloc((void *) data_buffer, bytes_read);

    auto buff = Buffer(data_buffer, (int)bytes_read);
    return buff;
}

/*
 * Will process one full buffer, this may contain more than one
 * rtmp packet so we iterate till every byte is processed
 */
void RTMPConnexion::receiveAndProcessConnect() {
    Buffer full_buffer = receive();
    // Sometimes we receive more than one packet(no more than 2) at the time, process all
    while(!full_buffer.is_end()) {
        // process RTMPHeaders
        auto rtmp_headers = RTMPHeaders(full_buffer.get_actual_position());
        // move header size in buffer to process body
        full_buffer.move_buffer((int) rtmp_headers.header_size);

        processBody(&full_buffer, &rtmp_headers);
        // move body size in buffer
        full_buffer.move_buffer((int) rtmp_headers.message_length);
    }
    full_buffer.free_buffer();
}

/*
 * process body message depending on message type id body will be different
 * Will only return 1 if processed body is and AMF connect packet
 */
void RTMPConnexion::processBody(Buffer* buff, RTMPHeaders* headers) {

    Logger::log(Logger::VERBOSE_LOG, &" Message type id" [ headers->message_type_id]);

    switch (headers->message_type_id) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            // Control messages
            Logger::log(Logger::VERBOSE_LOG, "Control message");
            break;
        case  17:
            // Command Message (AMF 3 encoded)
            Logger::log(Logger::VERBOSE_LOG, "Command Message AMF 3");
            break;
        case  20:
            // Command Message (AMF 0 encoded)
            Logger::log(Logger::VERBOSE_LOG, "Command Message AMF 0");
            AMFConnectCommand(buff, headers);
            break;
    }
}

/*
 * Process AMF command (should initially be connect command)
 * will save packet in object attribute
 */
void RTMPConnexion::AMFConnectCommand(Buffer* buff, RTMPHeaders* headers) {
    // decode packet
    AMFDataPacket packet;
    packet = AMF0Decoder::BlockDecoder(buff->get_actual_position(), (int)headers->message_length);
    // move byte errors of decoding
    buff->move_buffer(packet.byteError);

    packet.pprint();

    // get command
    std::string command;
    try{
        command = packet.getString("command");
        if (command != "connect"){return;}
        connect_packet = packet;
    } catch (NotFoundExcepcion &e){
        Logger::log(Logger::VERBOSE_LOG, e.get_error_message());
    }

}

/*
 * Send Buffer with window ACK size packet
 */
int RTMPConnexion::sendWindowACKSize() const {
    /* one byte basic header, type 0 message header, chunk stream = 2,
     * timestamp = 0, message_length = 4 bytes, message type 5 (WindowACk),
     * message stream id = 0
    */
    Buffer buff = RTMPHeaders::build_rtmp_header(BASIC_HEADER_TYPE::ONE_BYTE_HEADER, MESSAGE_HEADER_TYPE::TYPE0,
                                                 2, 0, 4, 5, 0);
    // 4 bytes for windowACK
    unsigned char _body[4];
    uint32_t setPeer = htonl(5000000);
    memcpy(_body, &setPeer, 4);
    buff.append((const char*)_body, 4);

    int res = (int)send(clientFD, buff.get_actual_position(), buff.get_size(), 0);

    buff.free_buffer();
    return res;
}

/*
 * Send Buffer setting peer bandwidth
 */
int RTMPConnexion::sendSetPeerBandwidth() const {
    /* one byte basic header, type 0 message header, chunk stream = 2,
 * timestamp = 0, message_length = 5 bytes, message type 6 (Peer Bandwidth),
 * message stream id = 0
*/
    Buffer buff = RTMPHeaders::build_rtmp_header(BASIC_HEADER_TYPE::ONE_BYTE_HEADER, MESSAGE_HEADER_TYPE::TYPE0,
                                                 2, 0, 5, 6, 0);
    // 4 bytes for windowACK
    unsigned char _body[5];
    uint32_t setPeer = htonl(5000000);
    memcpy(_body, &setPeer, 4);
    // 1 byte for limit type (2)
    _body[4] = 0x02;
    buff.append((const char*)_body, 5);

    int res = (int)send(clientFD, buff.get_actual_position(), buff.get_size(), 0);

    buff.free_buffer();
    return res;
}

/*
 * Send Result command in response to connect
 */
int RTMPConnexion::sendResulCommand() {

    return 0;
}

