/*
    rtmc_kins_scalar.h

    This kinematic solver simply applies a scale factor to each task-space
    pose value. There are the same number of joints (joint-space) as there
    are axes (task-space).
*/

#ifndef RTMC_KINS_SCALAR_H
#define RTMC_KINS_SCALAR_H

#ifdef __cplusplus
extern "C" {
#endif



#include "rtmc_path.h"



/*
    This function sets up the kinematic solver's parameters.
*/
void rtmc_kins_scalar_setup(const double* scale_factors);



/*
    This function loads a path. It must be called before
    `rtmc_kins_scalar_pose()` will output correct values for the given path.

    `rtmc_kins_scalar_setup()` must be called before this function will work.
*/
void rtmc_kins_scalar_load(rtmc_path_t path);



/*
    This function accepts an `s` parameter on the interval of [0, 1] and
    returns the joint-space pose of the machine along the loaded path.
    
    `rtmc_kins_scalar_load()` must be called before this function will work.
*/
void rtmc_kins_scalar_pose(double* pose, double s);



#ifdef __cplusplus
}
#endif

#endif // RTMC_KINS_SCALAR_H
