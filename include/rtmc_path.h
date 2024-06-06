/*
    rtmc_path.h
    
    This file declares `rtmc_path_t` (and related symbols) needed to use both
    the parser and interpolator.
*/

#ifndef RTMC_PATH_H
#define RTMC_PATH_H

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

typedef struct {
    bool is_valid;
    char* error_msg;
    double feed_rate;
    enum rtmc_path_type type;
    enum rtmc_plane plane;
    double coefficients[RTMC_NUM_AXES][RTMC_NUM_PATH_COEFFICIENTS];
} rtmc_path_t;




#ifdef __cplusplus
}
#endif

#endif // RTMC_PATH_H
