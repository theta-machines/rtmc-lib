/*
    parser/parser.c
*/

#include <ctype.h>
#include <stdlib.h>
#include "parser.h"
#include "rtmc_parser.h"

// global modal data
modal_data_t modal_data;
double feed_rate = 0;



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
    Parse a g-code string (called a block) and directly modify the
    `parsed_block` argument. Strings must be properly terminated with the
    null character ('\0').
*/
rtmc_parsed_block_t rtmc_parse(const char* block, const double* start_coords) {
    // object to be returned
    rtmc_parsed_block_t parsed_block;

    // non-modal data related to the block
    non_modal_data_t non_modal_data = {UNDEFINED_NON_MODAL_MODE, {0}};

    // g-code word (key/value pair)
    word_t word;

    // initialize FSM state
    state_t state = IDLE_STATE;

    // used to parse decimal values (later converted to doubles)
    char value_str[RTMC_MAX_DECIMAL_LENGTH + 1]; 
    int value_str_index = 0;

    // make path valid by default
    parsed_block.is_valid = true;

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
            else {
                // max value length has been exceeded
                // might truncate an exponent, so throw an error
                parsed_block.is_valid = false;
                parsed_block.error_msg = "Decimal value exceeded max length";
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
            bool valid_word = parse_word(&word, &non_modal_data, end_coords);
            if(!valid_word) {
                // flag error and stop parsing
                parsed_block.is_valid = false;
                parsed_block.error_msg = "Invalid G-code word";
                break;
            }
        }
        else if(state == ERROR_STATE) {
            // flag error and stop parsing
            parsed_block.is_valid = false;
            parsed_block.error_msg = "Grammar error in G-code block";
            break;
        }
        // note: there is no `else if(state == IDLE_STATE)` because
        // IDLE_STATE doesn't do anything
    }

    // if parsing was successful, generate the path
    if(parsed_block.is_valid) {
        generate_path(&parsed_block, start_coords, end_coords, non_modal_data);
    }

    return parsed_block;
}



/*
    A g-code block's meaning depends on modal data set by previous g-code 
    blocks. This function clears that data.
*/
void rtmc_flush_modal_data() {
    feed_rate = 0;
    modal_data.motion_mode = UNDEFINED_MOTION_MODE;
    modal_data.plane_mode = UNDEFINED_PLANE_MODE;
    modal_data.distance_mode = UNDEFINED_DISTANCE_MODE;
}
