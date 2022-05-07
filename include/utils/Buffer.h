#pragma once
#ifndef RTMPSERVER_BUFFER_H
#define RTMPSERVER_BUFFER_H


class Buffer {
    public:
        Buffer(const char*, int);
        void move_buffer(int bytes_to_move);
        [[nodiscard]] const char *get_actual_position() const;
        [[nodiscard]] int get_size() const;
        bool is_end();
        void append(const char*,int);
        void append(Buffer buff);

    private:
        const char* buffer_ini;
        const char* buffer_actual;
        int msg_size;
        int byte_error = 0;

};


#endif //RTMPSERVER_BUFFER_H
