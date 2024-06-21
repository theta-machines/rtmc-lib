/*
    rtmc_parser.h
*/

#ifndef RTMC_PARSER_H
#define RTMC_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif



#include <stdbool.h>
#include "rtmc_magic_numbers.h"
#include "rtmc_path.h"

enum rtmc_block_type {
    RTMC_BLOCK_TYPE_MODAL, RTMC_BLOCK_TYPE_PATH, RTMC_BLOCK_TYPE_DWELL,
    RTMC_BLOCK_TYPE_CANNED_CYCLE
    // TODO: implement these (or similar) for G28, etc.
    // RTMC_BLOCK_TYPE_HOME, RTMC_BLOCK_TYPE_PROBE
};



/*
    How to interpret this struct:
     * is_valid         indicates if the block was valid g-code
     * error_msg        provided whenever is_valid == false
     * block_type       classifies the g-code block
     * position_error   see note below (unit of meters)
     * dwell_time       the amount of time to pause before continuing

    position_error
    --------------
    Some g-code instructions (such as G02 & G03) are overconstrained, meaning
    that no path can meet all given constraints. In these cases, any
    discrepancy in the end position is reported.

    position_error is an array representing each axis's error as:
        position_error[i] = actual_end_position[i] - target_end_position[i]
    
    General Note
    ------------
    Not all of this data is applicable for a given g-code block. For example,
    if is_valid == false, you should ignore everything except for error_msg.
*/
typedef struct {
    bool is_valid;
    char* error_msg;
    enum rtmc_block_type type;
    double position_error[RTMC_NUM_AXES];
    double dwell_time;
} rtmc_parsed_block_t;



/*
    Parse a g-code string (called a block) and directly add to the path queue.
    G-code blocks must be terminated with '\r', '\n', or '\0'.

    Arguments:
    queue - the path queue
    block - a line of g-code

    A note on decimal values:
    Decimal values with more than RTMC_MAX_DECIMAL_LENGTH characters will be
    truncated. For long numbers, use "1E-18" instead of "0.000000000000000001".
*/
rtmc_parsed_block_t rtmc_parse(rtmc_path_queue_t* queue, const char* block);

/*
    A g-code block's meaning depends on previous g-code blocks.
    This function clears that data.
*/
void rtmc_flush_parser_data();



#ifdef __cplusplus
}
#endif

#endif // RTMC_PARSER_H
