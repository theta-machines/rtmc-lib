/*
    path.c
*/

#include <stdlib.h>
#include "rtmc_path.h"

// create a path queue
rtmc_path_queue_t rtmc_create_path_queue() {
    rtmc_path_queue_t queue = {NULL, NULL};
    return queue;
}

// adds a path to the queue
void rtmc_path_enqueue(rtmc_path_queue_t* queue, rtmc_path_t path) {
    rtmc_path_node_t* new_node = (rtmc_path_node_t*)malloc(sizeof(rtmc_path_node_t));
    new_node->path = path;
    new_node->next = NULL;

    if(queue->tail) { // queue has nodes
        queue->tail->next = new_node;
    }
    else { // queue is empty
        queue->head = new_node;
    }

    queue->tail = new_node;
}

// removes a path from the queue
rtmc_path_t rtmc_path_dequeue(rtmc_path_queue_t* queue) {
    if(queue->head) { // queue has nodes
        // get pointer to head node (to free later)
        rtmc_path_node_t* old_head = queue->head;

        // get head node's path
        rtmc_path_t path = queue->head->path;

        // remove node from linked list
        queue->head = queue->head->next;

        // handle empty queue
        if(queue->head == NULL) {
            queue->tail = NULL;
        }

        // free memory for dequeued node
        free(old_head);

        return path;
    }
    else { // queue is already empty
        // silently return an empty path
        rtmc_path_t path;
        return path;
    }
}

rtmc_path_t rtmc_path_queue_peek(const rtmc_path_queue_t* queue) {
    return queue->head->path;
}

// deletes all paths from the queue (freeing the memory)
void rtmc_flush_path_queue(rtmc_path_queue_t* queue) {
    // remove everything from the queue
    while(queue->head) {
        rtmc_path_dequeue(queue);
    }
}
