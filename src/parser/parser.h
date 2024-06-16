/*
    parser/parser.h

    THIS IS NOT A PUBLIC INTERFACE AND SHOULD NOT BE INCLUDED ANYWHERE
    EXCEPT FOR THE FILES WITHIN THIS DIRECTORY

    This header helps to break up the parser into several bite-sized files.
    These files and their purposes are described below.
     * parser.c --------- functions from `rtmc_parser.h` and FSM logic
     * parse_word.c ----- parse key/value pairs and update modal data
     * generate_path.c -- generates the pre-interpolated path
*/

#ifndef PARSER_H
#define PARSER_H



#include <stdbool.h>
#include "rtmc_parsed_block.h"

// holds a g-code word as key/value pair
typedef struct {
    char key;
    double value;
} word_t;



/*
    Below contains an enum for each modal group.

    The purpose of these enums is to document which G/M codes can be assigned
    as the active mode of a given group. Only one mode can be active for a
    given group. A mode remains active until changed by a subsequent g-code
    block. In this way, modes are persistent.

    The non-modal group is special. The active mode is not persistent and must
    be reset for each applicable g-code block.

    **CAUTION**
    Compile with `gcc -Wenum-conversion -Werror ...` (or similar) to catch
    erroneous values being assigned to the modal group enums.
*/

typedef enum {
    UNDEFINED_MOTION_MODE,
    G00, G01, G02, G03
} motion_mode_t;

typedef enum {
    UNDEFINED_PLANE_MODE,
    G17, G18, G19
} plane_mode_t;

typedef enum {
    UNDEFINED_DISTANCE_MODE,
    G90, G91
} distance_mode_t;

typedef enum {
    UNDEFINED_NON_MODAL_MODE,
    G04
} non_modal_mode_t;

// this struct groups the modal data together for use within the parser
typedef struct {
    motion_mode_t motion_mode;
    plane_mode_t plane_mode;
    distance_mode_t distance_mode;
} modal_data_t;

// this struct groups all non-modal data together for use within the parser
typedef struct {
    non_modal_mode_t mode;
    double relative_offset[3];
} non_modal_data_t;



/*
    Global modal data
*/
extern modal_data_t modal_data;
extern double feed_rate;



/*
    Private interface
*/
bool parse_word (
    word_t* word,
    non_modal_data_t* non_modal_data,
    double* end_coords
    );

void generate_path(
    rtmc_parsed_block_t* parsed_block,
    const double* start_coords,
    const double* end_coords,
    const non_modal_data_t non_modal_data
    );



#endif // PARSER_H
