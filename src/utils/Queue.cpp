#include <stdlib.h>
#include "utils/Queue.h"


node_t* head = nullptr;
node_t* tail = nullptr;

// will queue a new request(from tail)
// afterwards threads will consume with dequeue
void Queue::enqueue(int *client_socket) {
    auto* new_request = (node_t*)malloc(sizeof(node_t));
    new_request->client_socket = client_socket;
    new_request->next = nullptr;
    if (tail == nullptr) {
        head = new_request;
    }
    else{
        tail->next = new_request;
    }
    tail = new_request;
}

// Thread consumes(from head) from the queue,
// will take a client socket to process the request
int* Queue::dequeue() {
    if (head == nullptr) {
        return nullptr;
    }
    int* result = head->client_socket;
    head = head->next;
    if (head == nullptr){
        tail = nullptr;
    }
    return result;
}
