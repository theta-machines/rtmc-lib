/*
    parser/parse_word.c
*/

#include "parser.h"
#include "rtmc_math.h"
#include "rtmc_parser.h"

/*
    Parse the key/value pairs (a.k.a., words) and update the modal data

    Note: some keys have multiple meanings based on context. For example,
    'P' is typically an axis, but can be the dwell time if G04 is active.

    Returns `true` for valid words and `false` for invalid words. 
    `modal_data`, `feed_rate`, and `end_coords` are all passed by reference
    and can be modified by this function.
*/
bool parse_word(word_t* word) {

    if(word->key == 'A') // A-words
        modal_data.end_coords[RTMC_A_AXIS] = word->value;
    
    else if(word->key == 'B') // B-words
        modal_data.end_coords[RTMC_B_AXIS] = word->value;
    
    else if(word->key == 'C') // C-words
        modal_data.end_coords[RTMC_C_AXIS] = word->value;
    
    else if(word->key == 'F') // F-words
        feed_rate = word->value;
    
    else if(word->key == 'G') { // G-words
        if(rtmc_is_equal(word->value, 0)) // G00 word
            modal_data.motion_mode = G00;
        
        else if(rtmc_is_equal(word->value, 1)) // G01 word
            modal_data.motion_mode = G01;
        
        else if(rtmc_is_equal(word->value, 2)) // G02 word
            modal_data.motion_mode = G02;
        
        else if(rtmc_is_equal(word->value, 3)) // G03 word
            modal_data.motion_mode = G03;
        
        else if(rtmc_is_equal(word->value, 17)) // G17 word
            modal_data.plane_mode = G17;
        
        else if(rtmc_is_equal(word->value, 18)) // G18 word
            modal_data.plane_mode = G18;
        
        else if(rtmc_is_equal(word->value, 19)) // G19 word
            modal_data.plane_mode = G19;
        
        else if(rtmc_is_equal(word->value, 90)) // G90 word
            modal_data.distance_mode = G90;
        
        else if(rtmc_is_equal(word->value, 91)) // G91 word
            modal_data.distance_mode = G91;
        
        else // unrecognized value
            return false;
    }
    else if(word->key == 'I')
        non_modal_data.relative_offset[RTMC_X_AXIS] = word->value;
    
    else if(word->key == 'J')
        non_modal_data.relative_offset[RTMC_Y_AXIS] = word->value;

    else if(word->key == 'K')
        non_modal_data.relative_offset[RTMC_Z_AXIS] = word->value;
        
    else if(word->key == 'P') { // P-words
        // TODO: can also be a parameter
        // use if(modal_data.motion_mode == SOMETHING) to determine behavior
        modal_data.end_coords[RTMC_P_AXIS] = word->value;
    }
    else if(word->key == 'Q') { // Q-words
        // TODO: can also be a parameter
        modal_data.end_coords[RTMC_Q_AXIS] = word->value;
    }
    else if(word->key == 'R') { // R-words
        // TODO: can also be a parameter
        modal_data.end_coords[RTMC_R_AXIS] = word->value;
    }

    else if(word->key == 'U') // U-words
        modal_data.end_coords[RTMC_U_AXIS] = word->value;
    
    else if(word->key == 'V') // V-words
        modal_data.end_coords[RTMC_V_AXIS] = word->value;
    
    else if(word->key == 'W') // W-words
        modal_data.end_coords[RTMC_W_AXIS] = word->value;
    
    else if(word->key == 'X') // X-words
        modal_data.end_coords[RTMC_X_AXIS] = word->value;
    
    else if(word->key == 'Y') // Y-words
        modal_data.end_coords[RTMC_Y_AXIS] = word->value;
    
    else if(word->key == 'Z') // Z-words
        modal_data.end_coords[RTMC_Z_AXIS] = word->value;
    
    else // unrecognized key
        return false;

    // no errors found
    return true;
}
