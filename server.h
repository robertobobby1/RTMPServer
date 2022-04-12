#ifndef RTMPSERVER_SERVER_H
#define RTMPSERVER_SERVER_H

void check(int socket, const char *err);

[[noreturn]] void* handle_connection(void* arg);
void start_socket();
void initialize_threads();


#endif //RTMPSERVER_SERVER_H
