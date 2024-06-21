/*
    rtmc_path.h
*/

#ifndef RTMC_PATH_H
#define RTMC_PATH_H

#ifdef __cplusplus
extern "C" {
#endif



#include "rtmc_magic_numbers.h"

/*
    The `coefficients` relate to a given form (either polynomial or 
    trigonometric). The form is selected based on the path `type`.

    RTMC_POLYNOMIAL uses the form p(s) = As^3 + Bs^2 + Cs + D

    RTMC_TRIGONOMETRIC uses the form p(s) = A * sin(B * (s - C)) + D

    RTMC_HELICAL_XY uses the trigonometric form for the X and Y axes, and the
    polynomial form for all others. 
*/
enum rtmc_path_type {
    RTMC_PATH_TYPE_POLYNOMIAL, RTMC_PATH_TYPE_TRIGONOMETRIC,
    RTMC_PATH_TYPE_HELICAL_XY, RTMC_PATH_TYPE_HELICAL_XZ,
    RTMC_PATH_TYPE_HELICAL_YZ
};

typedef struct {
    enum rtmc_path_type type;
    double feed_rate;
    double coefficients[RTMC_NUM_AXES][RTMC_NUM_PATH_COEFFICIENTS];
} rtmc_path_t;

typedef struct rtmc_path_node {
    rtmc_path_t path;
    struct rtmc_path_node* next;
} rtmc_path_node_t;

typedef struct {
    rtmc_path_node_t* head;
    rtmc_path_node_t* tail;
} rtmc_path_queue_t;



// create a path queue
rtmc_path_queue_t rtmc_create_path_queue();

// adds a path to the queue
void rtmc_path_enqueue(rtmc_path_queue_t* queue, rtmc_path_t path);

// removes and returns a path from the queue
rtmc_path_t rtmc_path_dequeue(rtmc_path_queue_t* queue);

// returns the head of the queue without removing it
rtmc_path_t rtmc_path_queue_peek(const rtmc_path_queue_t* queue);

// deletes all paths from the queue (freeing the memory)
void rtmc_flush_path_queue(rtmc_path_queue_t* queue);



#ifdef __cplusplus
}
#endif

#endif // RTMC_PATH_H
