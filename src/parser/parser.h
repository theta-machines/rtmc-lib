/*
    parser/parser.h

    THIS IS NOT A PUBLIC INTERFACE AND SHOULD NOT BE INCLUDED ANYWHERE
    EXCEPT FOR THE FILES WITHIN THIS DIRECTORY

    This header helps to break up the parser into several bite-sized files.
    These files and their purposes are described below.
     * parser.c --------- functions from `rtmc_parser.h` and FSM logic
     * parse_word.c ----- parse key/value pairs and update modal data
     * generate_path.c -- generates the joint-space path
*/

#ifndef PARSER_H
#define PARSER_H



#include <stdbool.h>
#include "rtmc_parser.h"
#include "rtmc_path.h"

// magic numbers
#define NUM_RELATIVE_OFFSETS 3


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

enum motion_mode {
    UNDEFINED_MOTION_MODE,
    G00, G01, G02, G03
};

enum plane_mode {
    UNDEFINED_PLANE_MODE,
    G17, G18, G19
};

enum distance_mode {
    UNDEFINED_DISTANCE_MODE,
    G90, G91
};

enum non_modal_mode {
    UNDEFINED_NON_MODAL_MODE,
    G04
};

// this struct groups the modal data together for use within the parser
typedef struct {
    enum motion_mode motion_mode;
    enum plane_mode plane_mode;
    enum distance_mode distance_mode;
} modal_data_t;

// this struct groups all non-modal data together for use within the parser
typedef struct {
    enum non_modal_mode mode;
    double relative_offset[NUM_RELATIVE_OFFSETS];
} non_modal_data_t;



/*
    Global modal data
*/
extern modal_data_t modal_data;
extern non_modal_data_t non_modal_data;
extern double feed_rate;
extern double start_coords[RTMC_NUM_AXES];
extern double end_coords[RTMC_NUM_AXES];



/*
    Holds a g-code word as key/value pair
*/
typedef struct {
    char key;
    double value;
} word_t;



/*
    Private interface
*/
// return false when for invalid words; otherwise assigns key/value to `word`
bool parse_word(word_t* word);

// builds the `path` and `parsed_block`
void generate_path(rtmc_path_t* path, rtmc_parsed_block_t* parsed_block);



#endif // PARSER_H
