
#include "Server.h"
#include "utils/Logger.h"
#include "Excepcions/SocketExcepcion.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

// structs for ip address and port
SA_IN server_addr;

Server::Server(int port, int backlog) :
    _port(port), _backlog(backlog){}

bool Server::initializeServer() {
    try{
        initializeServerHandler();
        return true;
    } catch (const SocketException &e){
        Logger::log(Logger::SOCKET_ERROR_LOG, ((std::string)("retrying socket initialisation in 10 seconds\n") +
                 e.get_error_message())
                .c_str());
        return false;
    }
}

/*
 * try to initialize server if not exception thrown
 */
void Server::initializeServerHandler(){
    // Create a socket
    check(_server_fd = socket(AF_INET, SOCK_STREAM, 0),
          "Couldn't create socket");

    // Set server address port and ip address in struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(_port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the ip address and port to a socket
    check(bind(_server_fd, (SA*)&server_addr, sizeof(server_addr)),
          "Couldn't bind to specified port and ip address");

    // strart listening on selected  port and ip address
    check(listen(_server_fd, _backlog),
          "Couldn't listen");
    Logger::log(Logger::SOCKET_LOG, "socket binded and listening \n");
}

/*
 * checks errors, if there is an error will output message
 * and return true for error or false for none
 */
void Server::check(int socket, const char *err) {
    if (socket == (-1)){
        throw SocketException(err);
    }
}

/*
 * Accepts new connections on socket returns client fd or
 * -1 on error
 */
int Server::acceptConnection() const {
    int* _client_socket = (int*) malloc(sizeof(int));
    int addr_size = sizeof(SA_IN);

    int client_socket = accept(_server_fd, (sockaddr *) _client_socket, (socklen_t *) &addr_size);
    Logger::log(Logger::SOCKET_LOG, "New connection!\n");
    free(_client_socket);

    return client_socket;
}
