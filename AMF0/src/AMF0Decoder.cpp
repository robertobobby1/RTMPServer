#include <vector>
#include <netinet/in.h>
#include <cstring>
#include "AMF0Decoder.h"
#include "AMFDataPacket.h"

#define ntohll(x) ((1==ntohl(1)) ? (x) : ((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))


/*
 * Considers network data (little-endian) and passes this to big-endian for multibyte values
 * One by one process full AMF0 packet
 * Returns full AMFDataPacket
 */
AMFDataPacket AMF0Decoder::BlockDecoder(const char *buffer, int length) {
    AMF0Decoder decoder;
    AMFDataPacket decoded_packet;
    // object counter to find in maps afterwards
    int obj_num = 0; int string_num = 0; int number_num = 0; int bool_num = 0;

    while(decoder.processed < length){
        decoder.processed++;
        switch (buffer[decoder.processed-1]) {
            case AMF_DOUBLE:
                // first number always transaction_id
                if (number_num == 0) {decoded_packet.add("transaction_id", decoder.processDouble(buffer));}
                else {decoded_packet.add("number-" + std::to_string(number_num), decoder.processDouble(buffer));}
                number_num++;
                break;
            case AMF_BOOL:
                decoded_packet.add("boolean-" + std::to_string(bool_num), decoder.processBoolean(buffer));
                bool_num++;
                break;
            case AMF_STRING:
                // first string always command
                if (string_num == 0) {decoded_packet.add("command", decoder.processString(buffer));}
                else {decoded_packet.add("string-" + std::to_string(string_num), decoder.processString(buffer));}
                string_num++;
                break;
            case AMF_OBJECT:
                decoded_packet.add(decoder.processObject(buffer, length));
                obj_num++;
                break;
            case AMF_NULL:
                decoder.processed++;
                break;
            default:
                //error
                break;
        }
    }
    decoded_packet.byteError = decoder.byteError;
    return decoded_packet;
}

void AMF0Decoder::UniTypeDecoder(const char *buffer) {
    switch (buffer[0]) {
        case '\000':
            processDouble(buffer);
            break;
        case '\001':
            processBoolean(buffer);
            break;
        case '\002':
            processString(buffer);
            break;
        case '\003':
            //processObject(buffer);
            break;
        case '\005':
            processed++;
            break;
        default:
            //error
            break;
    }
}
/*
 * Processes one string, first reads two bytes(represents length of string)
 * Then updates processed class attr and returns string
 */
std::string AMF0Decoder::processString(const char *buffer) {

    uint16_t *x; x = (uint16_t*) malloc(sizeof(uint16_t));
    memcpy(x, &buffer[processed], sizeof(uint16_t));
    int string_length = ntohs(*x);

    char *temp = (char *)(malloc(string_length));

    memcpy(temp, &buffer[processed+2], string_length);

    // 2 byte string length and string length
    processed += string_length + 2;
    std::string ret(temp, string_length);
    free(temp);free(x);

    return ret;
}

/*
 * Return object* so that memory can be freed
 * Method iterates full object from start (0x03) till end (0x000009)
 * and adds pair values to an object struct
 */
AMFDataPacket AMF0Decoder::processObject(const char *buffer, int length) {
    AMFDataPacket ret;
    std::string key;

    // each iteration will process one key-value pair
    while (!isEndObject(buffer) && length > processed){
        // key is always string
        key = processString(buffer);
        processed++;
        switch (buffer[processed-1]) {
            case AMF_DOUBLE:
                ret.add(key, processDouble(buffer));
                break;
            case AMF_BOOL:
                ret.add(key, processBoolean(buffer));
                break;
            case AMF_STRING:
                ret.add(key, processString(buffer));
                break;
            case AMF_NULL:
                processed++;
                break;
            default:
                break;
        }
    }
    // 3 last bytes 0x00, 0x00, 0x09(end block)
    processed += 3;
    return ret;
}


/*
 * Always 64 bits(8 bytes) with double precision floating point
 */
double AMF0Decoder::processDouble(const char *buffer) {
    uint64_t *x;x = (uint64_t*)(malloc(sizeof(uint64_t)));

    // Random vlc media player bug
    if (buffer[processed + 7] == '\303'){
        // exchange next byte with it (add 1 to processed)
        memcpy((void *) &buffer[processed + 7], &buffer[processed + 8], sizeof(buffer[processed + 8]));
        processed++; byteError++;
        // processed - 1 because of extra byte added before(as processed)
        memcpy(x, &buffer[processed-1], 8);
    } else{
        memcpy(x, &buffer[processed], 8);
    }

    *x = ntohll(*x);
    double *m; m = (double*)(x);
    processed += 8;
    return *m;
}

/*
 * Always one byte true set to 0x01 and false set to 0x00
 */
bool AMF0Decoder::processBoolean(const char *buffer) {
    processed++;
    if (buffer[processed] != '\001')
        return false;
    return true;
}

bool AMF0Decoder::isEndObject(const char *buffer) const {
    return buffer[processed] == '\000' &&
           buffer[processed+1] == '\000' &&
           buffer[processed+2] == AMF_END_OBJECT;
}
