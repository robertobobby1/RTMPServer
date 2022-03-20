#include <vector>
#include <netinet/in.h>
#include <cstring>
#include "AMF0Decoder.h"

#define ntohll(x) ((1==ntohl(1)) ? (x) : ((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))


/*
 * Considers network data (little-endian) and passes this to big-endian for multibyte values
 * One by one process full AMF0 packet
 */
AMFPacket AMF0Decoder::BlockDecoder(const char *buffer, int length) {
    AMF0Decoder decoder; AMFPacket decoded_packet;
    int obj_num = 0;

    while(decoder.processed < length){
        decoder.processed++;
        switch (buffer[decoder.processed-1]) {
            case AMF_DOUBLE:
                decoded_packet.transaction_id =
                        decoder.processDouble(buffer);
                break;
            case AMF_BOOL:
                decoder.processBoolean(buffer);
                break;
            case AMF_STRING:
                decoded_packet.command =
                        decoder.processString(buffer);
                break;
            case AMF_OBJECT:
                decoded_packet.objects[obj_num] = decoder.processObject(buffer);
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
            processObject(buffer);
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

    char *temp = (char *)(malloc(sizeof(char) * string_length));

    memcpy(temp, &buffer[processed+2], string_length);

    // 2 byte string length and string length
    processed += string_length + 2;
    std::string ret(temp);
    free(temp);free(x);

    return ret;
}

/*
 * Return object* so that memory can be freed
 * Method iterates full object from start (0x03) till end (0x000009)
 * and adds pair values to an object struct
 */
Object AMF0Decoder::processObject(const char *buffer) {
    Object ret;
    //ret = (Object*)malloc(sizeof(Object)*10);
    int obj_num = 0;

    // each iteration will process one key-value pair
    while (!isEndObject(buffer)){
        // key is always string
        ret.object[obj_num].key = processString(buffer);
        processed++;
        switch (buffer[processed-1]) {
            case AMF_DOUBLE:
                ret.object[obj_num].value = std::to_string(processDouble(buffer));
                break;
            case AMF_BOOL:
                ret.object[obj_num].value = std::to_string(processBoolean(buffer));
                break;
            case AMF_STRING:
                ret.object[obj_num].value = processString(buffer);
                break;
            case AMF_NULL:
                processed++;
                break;
            default:
                break;
        }
        obj_num++;
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
    memcpy(x, &buffer[processed], 8);
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
    return buffer[processed] != '\000' &&
           buffer[processed] != '\000' &&
           buffer[processed] != AMF_END_OBJECT;
}
