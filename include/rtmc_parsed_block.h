/*
    rtmc_parsed_block.h
    
    This file declares `rtmc_parsed_block_t` (and related symbols) needed to
    use both the parser and interpolator.
*/

#ifndef RTMC_PARSED_BLOCK_H
#define RTMC_PARSED_BLOCK_H

#ifdef __cplusplus
extern "C" {
#endif



#include <stdbool.h>
#include "rtmc_magic_numbers.h"

enum rtmc_axes {
    RTMC_X_AXIS, RTMC_Y_AXIS, RTMC_Z_AXIS,
    RTMC_U_AXIS, RTMC_V_AXIS, RTMC_W_AXIS,
    RTMC_P_AXIS, RTMC_Q_AXIS, RTMC_R_AXIS,
    RTMC_A_AXIS, RTMC_B_AXIS, RTMC_C_AXIS
};

enum rtmc_path_type {
    RTMC_POLYNOMIAL,
    RTMC_TRIGONOMETRIC,
    RTMC_HELICAL
};

enum rtmc_plane {
    RTMC_XY_PLANE,
    RTMC_XZ_PLANE,
    RTMC_YZ_PLANE
};

/*
    How to interpret this struct:
     * is_valid         indicates if the block was valid g-code
     * error_msg        provided whenever is_valid == false
     * is_path          indicates if the block requires motion
     * feed_rate        the path's feed rate in m/s
     * path_type        the path's type of interpolation
     * path_plane       the path's selected plane
     * coefficients     the path's coefficient matrix
     * position_error   see note below (unit of meters)

    position_error
    --------------
    Some g-code instructions are overconstrained. For example, the "IJK" form
    of the G02/G03 instructions require setting the center point of the arc
    and the end point. If the distance from the end point to the center is
    different than the distance from the start point to the center, then there
    is no true arc that can be formed. We can't simply throw an error because
    tiny differences in these lengths might be within the program's tolerance.
    The solution is to report any discrepancies so that users of this library
    can determine if the position_error violates their tolerances or not.
    
    General Note
    ------------
    Not all of this data is applicable for a given g-code block. For example,
    if is_path == false, then you should ignore feed_rate, path_type, etc. If
    is_valid == false, you should ignore everything except for error_msg.
*/
typedef struct {
    bool is_valid;
    char* error_msg;
    bool is_path;
    double feed_rate;
    enum rtmc_path_type path_type;
    enum rtmc_plane path_plane;
    double coefficients[RTMC_NUM_AXES][RTMC_NUM_PATH_COEFFICIENTS];
    double position_error;
    // TODO: implement these (or similar) for canned cycles, macros, etc.
    // bool is_macro;
    // char macro_key;
    // double macro_value;
} rtmc_parsed_block_t;



#ifdef __cplusplus
}
#endif

#endif // RTMC_PARSED_BLOCK_H
