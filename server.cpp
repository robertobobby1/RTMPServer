#include "rtmp/RTMPServer.h"

int main(){
    // run full application
    try{
        RTMPServer server;
        server.run();
    } catch(std::exception &exception){
        exception.what();
    }
}
