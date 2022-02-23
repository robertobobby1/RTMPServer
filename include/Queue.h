#ifndef RTMPSERVER_QUEUE_H
#define RTMPSERVER_QUEUE_H

struct node{
    struct node* next;
    int* client_socket;
};
typedef struct node node_t;

class Queue {

    public:
        static void enqueue(int *client_socket);
        static int* dequeue();
};


#endif //RTMPSERVER_QUEUE_H
