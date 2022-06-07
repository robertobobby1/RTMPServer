#include <cstring>
#include <cstdlib>
#include "utils/Buffer.h"

/*
 * Creates buffer with pointer to beginning and msgsize of buffer
 */
Buffer::Buffer(const char* buffer, int msgsize)
    : buffer_ini(buffer), buffer_actual(buffer), msg_size(msgsize) {}

/*
 * Positions buffer moving it bytes_to_move bytes
 */
void Buffer::move_buffer(int bytes_to_move) {
    buffer_actual = &buffer_actual[bytes_to_move];
}

/*
 * returns actual buffer position
 */
const char* Buffer::get_actual_position() const{
    return buffer_actual;
}

/*
 * returns message size of buffer
 */
int Buffer::get_size() const {
    return msg_size;
}

/*
 * Returns true if full buffer is processed
 * otherwise false
 */
bool Buffer::is_end(){
    return buffer_actual == &buffer_ini[msg_size];
}

/*
 * Appends at the end of the buffer (makes it bigger)
 */
void Buffer::append(const char *extra_buff, int length) {

    // get actual position to reposition on new reallocated buffer
    std::ptrdiff_t bytes = ((char *)buffer_actual) - ((char *)buffer_ini);
    // allocate new buffer size
    buffer_ini = (const char*) realloc((void *) buffer_ini, msg_size + length);
    buffer_actual = &buffer_ini[bytes];
    // copy both buffers one after the other
    memcpy((void *) &buffer_ini[msg_size], extra_buff, length);
    msg_size = msg_size + length;
}

/*
 * Appends another buffer to original
 * Destructive (will free "another" buffer) only keeping original
 */
void Buffer::append(Buffer buff){
    if (buff.buffer_ini == nullptr){
        return;
    }
    append(buff.buffer_ini, buff.msg_size);
    buff.free_buffer();
}

/*
 * Free buffer pointer to avoid leaks, this method should
 * always be called, for each buffer
 */
void Buffer::free_buffer() {
    free((void *) buffer_ini);
}
