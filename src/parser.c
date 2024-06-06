/*
    parser.c
*/

#include <ctype.h> 
#include <stdbool.h>
#include <stdlib.h>
#include "rtmc_magic_numbers.h"
#include "rtmc_parser.h"



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

// This struct groups the modal data together for use within the parser
typedef struct {
    motion_mode_t motion_mode;
    plane_mode_t plane_mode;
    distance_mode_t distance_mode;
} modal_data_t;



/*
    Parse the key/value pairs (a.k.a., words) and update the modal data

    Note: some keys have multiple meanings based on context. For example,
    'P' is typically an axis, but can be the dwell time if G04 is active.

    Returns `true` for valid words and `false` for invalid words. 
    `modal_data`, `feed_rate`, and `end_coords` are all passed by reference
    and can be modified by this function.
*/
bool parse_word (
    const word_t* word,
    modal_data_t* modal_data,
    double* feed_rate,
    double* end_coords
    ) {

    if(word->key == 'A') { // A-words
        end_coords[RTMC_A_AXIS] = word->value;
    }
    else if(word->key == 'B') { // B-words
        end_coords[RTMC_B_AXIS] = word->value;
    }
    else if(word->key == 'C') { // C-words
        end_coords[RTMC_C_AXIS] = word->value;
    }
    else if(word->key == 'F') { // F-words
        *feed_rate = word->value;
    }
    else if(word->key == 'G') { // G-words
        if(word->value == 0) { // G00 word
            modal_data->motion_mode = G00;
        }
        else if(word->value == 1) { // G01 word
            modal_data->motion_mode = G01;
        }
        else {
            // unrecognized value
            return false;
        }
    }
    else if(word->key == 'P') { // P-words
        // TODO: can also be a parameter
        // use if(modal_data->motion_mode == SOMETHING) to determine behavior
        end_coords[RTMC_P_AXIS] = word->value;
    }
    else if(word->key == 'Q') { // Q-words
        // TODO: can also be a parameter
        end_coords[RTMC_Q_AXIS] = word->value;
    }
    else if(word->key == 'R') { // R-words
        // TODO: can also be a parameter
        end_coords[RTMC_R_AXIS] = word->value;
    }
    else if(word->key == 'U') { // U-words
        end_coords[RTMC_U_AXIS] = word->value;
    }
    else if(word->key == 'V') { // V-words
        end_coords[RTMC_V_AXIS] = word->value;
    }
    else if(word->key == 'W') { // W-words
        end_coords[RTMC_W_AXIS] = word->value;
    }
    else if(word->key == 'X') { // X-words
        end_coords[RTMC_X_AXIS] = word->value;
    }
    else if(word->key == 'Y') { // Y-words
        end_coords[RTMC_Y_AXIS] = word->value;
    }
    else if(word->key == 'Z') { // Z-words
        end_coords[RTMC_Z_AXIS] = word->value;
    }
    else {
        // unrecognized key
        return false;
    }

    // no errors found
    return true;
}



/*
    This function outputs a path based on parsed data



    Determining path coefficients:

        x(s)
         Λ
         |
    x(1) +           *
         |        *
         |     *
         |  *
    x(0) +-----------+--> s
         0           1

    s is always normalized on the interval [0, 1].
    x(0) is the axis's start_coordinate.
    x(1) is the axis's end_coordinate.

    For a line in the form x(s) = As^3 + Bs^2 + Cs + D,
    A = 0
    B = 0
    C = x(1) - x(0)
    D = x(0)

    This can be derived using point-slope form of the line. Note that each
    axis has its own unique line x(s), y(s), z(s), ..., and so on. The same
    derivation method applies to all polynomial interpolation methods (cubic
    and quartic).

    For trigonometric interpolation methods, the form x(s) = Asin(Bs + C) + D
    is used.

    For helical interpolation, some axes are in the polynomial form, while
    others are in the trigonometric form. This relationship is implicit based
    on the selected plane. For a downward spiral (cutting into the XY plane),
    the Z-Axis would be polynomial-type (moving in a straight line), while the
    X and Y axes would be trigonometric-type (moving together in a circle).
*/
void generate_path(
    const double* start_coords, 
    const double* end_coords, 
    const double feed_rate, 
    const modal_data_t* modal_data, 
    rtmc_path_t* path
    ) {
    if(modal_data->motion_mode == G00) {
        path->type = RTMC_POLYNOMIAL;
        path->feed_rate = RTMC_RAPID_RATE;

        // set coefficients (polynomial, linear)
        for(int i = 0; i < RTMC_NUM_AXES; i++) {
            path->coefficients[i][0] = 0;
            path->coefficients[i][1] = 0;
            path->coefficients[i][2] = end_coords[i] - start_coords[i];
            path->coefficients[i][3] = start_coords[i];
        }
    }
    else if(modal_data->motion_mode == G01) {
        if(feed_rate > 0) {
            path->type = RTMC_POLYNOMIAL;
            path->feed_rate = feed_rate;

            // set coefficients (polynomial, linear)
            for(int i = 0; i < RTMC_NUM_AXES; i++) {
                path->coefficients[i][0] = 0;
                path->coefficients[i][1] = 0;
                path->coefficients[i][2] = end_coords[i] - start_coords[i];
                path->coefficients[i][3] = start_coords[i];
            }   
        }
        else {
            // invalid feed rate, invalidate the path
            path->is_valid = false;
            path->error_msg = "Feed rate is zero or negative";
        }
    }
    else {
        // unknown motion mode, invalidate the path
        path->is_valid = false;
        path->error_msg = "Unknown motion mode";
    }
}



// FSM states
typedef enum {
    IDLE_STATE,
    KEY_STATE,
    VALUE_STATE,
    PARSE_STATE,
    ERROR_STATE // only catches grammar errors
} state_t;

/*
    FSM input char types

    E - 'e', 'E' (special since it can be either a letter or digit)
    letter - a-z, A-Z (excluding 'e' and 'E')
    digit - 0-9, '.', '+', '-'
    empty - ' ', '\t', '\n', '\r', '\0'
    invalid - anything else
*/
typedef enum {
    E_TYPE,
    LETTER_TYPE,
    DIGIT_TYPE,
    EMPTY_TYPE,
    INVALID_TYPE
} char_type_t;



/*
    Classify the FSM input chars
*/
char_type_t get_char_type(char c) {

    if(c == 'e' || c == 'E')
        return E_TYPE;

    else if(isalpha(c))
        return LETTER_TYPE;

    else if(isdigit(c) || c == '.' || c == '+' || c == '-') 
        return DIGIT_TYPE;

    else if(c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\0')
        return EMPTY_TYPE;

    else
        return INVALID_TYPE;
}



/*
    Return the FSM's next state based on the current state and
    input character's type.
*/
state_t get_next_state(state_t current_state, char c) {
    char_type_t char_type = get_char_type(c);

    switch(current_state) {
        case IDLE_STATE:
            switch(char_type) {
                case E_TYPE: return KEY_STATE;
                case LETTER_TYPE: return KEY_STATE;
                case DIGIT_TYPE: return ERROR_STATE;
                case EMPTY_TYPE: return IDLE_STATE;
                case INVALID_TYPE: return ERROR_STATE;
            }

        case KEY_STATE:
            switch(char_type) {
                case E_TYPE: return ERROR_STATE;
                case LETTER_TYPE: return ERROR_STATE;
                case DIGIT_TYPE: return VALUE_STATE;
                case EMPTY_TYPE: return ERROR_STATE;
                case INVALID_TYPE: return ERROR_STATE;
            }

        case VALUE_STATE:
            switch(char_type) {
                case E_TYPE: return VALUE_STATE;
                case LETTER_TYPE: return ERROR_STATE;
                case DIGIT_TYPE: return VALUE_STATE;
                case EMPTY_TYPE: return PARSE_STATE;
                case INVALID_TYPE: return ERROR_STATE;
            }

        case PARSE_STATE:
            switch(char_type) {
                case E_TYPE: return KEY_STATE;
                case LETTER_TYPE: return KEY_STATE;
                case DIGIT_TYPE: return ERROR_STATE;
                case EMPTY_TYPE: return IDLE_STATE;
                case INVALID_TYPE: return ERROR_STATE;
            }

        case ERROR_STATE:
            return ERROR_STATE;
    }
}



/*
    Parse a g-code string (called a block) and directly modify the `path` 
    argument. Strings must be properly terminated with the null character 
    ('\0').
*/
void rtmc_parse(const char* block, const double* start_coords, rtmc_path_t* path) {
    // persistent data
    static modal_data_t modal_data;
    static double feed_rate = 0;

    // g-code word (key/value pair)
    word_t word;

    // initialize FSM state
    state_t state = IDLE_STATE;

    // used to parse decimal values (later converted to doubles)
    char value_str[RTMC_MAX_DECIMAL_LENGTH + 1]; 
    int value_str_index = 0;

    // make path valid by default
    path->is_valid = true;

    // initialize end coordinates
    double end_coords[RTMC_NUM_AXES];
    for(int i = 0; i < RTMC_NUM_AXES; i++) {
        end_coords[i] = start_coords[i];
    }

    // loop until the end of the line, counting number of iterations
    bool end_of_line = false;
    for(int i = 0; !end_of_line; i++) {

        // isolate the current character
        char c = block[i];

        // setting `end_of_line` after checking the loop's exit condition
        // ensures there is an iteration that processes the terminating char
        end_of_line = (c == '\r' || c == '\n' || c == '\0');

        // update state (this is safe on the first iteration because 
        // IDLE_STATE doesn't do anything)
        state = get_next_state(state, c);

        if(state == KEY_STATE) {
            // store the char as a key
            word.key = toupper(c);
        }
        else if(state == VALUE_STATE) {
            // build `value_str` until max length is reached
            if(value_str_index < RTMC_MAX_DECIMAL_LENGTH) {
                value_str[value_str_index++] = c;
            }
        }
        else if(state == PARSE_STATE) {
            // reset value_str null characters after index
            for(int i = value_str_index; i < RTMC_MAX_DECIMAL_LENGTH; i++) {
                value_str[i] = '\0';
            }

            // reset value_str_index
            value_str_index = 0;

            // convert value_str to double
            word.value = strtod(value_str, NULL); // TODO: strtod() could use error handling

            // update modal data based on new g-code word (key/value pair)
            bool valid_word = parse_word(&word, &modal_data, &feed_rate, end_coords);
            if(!valid_word) {
                // flag error and stop parsing
                path->is_valid = false;
                path->error_msg = "Invalid G-code word";
                break;
            }
        }
        else if(state == ERROR_STATE) {
            // flag error and stop parsing
            path->is_valid = false;
            path->error_msg = "Grammar error in G-code block";
            break;
        }
        // note: there is no `else if(state == IDLE_STATE)` because
        // IDLE_STATE doesn't do anything
    }

    // if parsing was successful, generate the path
    if(path->is_valid) {
        generate_path(start_coords, end_coords, feed_rate, &modal_data, path);
    }
}
