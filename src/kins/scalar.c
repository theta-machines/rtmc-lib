/*
    kins/scalar.c
*/

#include <math.h>
#include "rtmc_kins_scalar.h"
#include "rtmc_magic_numbers.h"


static rtmc_path_t scaled_path;
static double scale_factors[RTMC_NUM_AXES];



void rtmc_kins_scalar_setup(const double* sf) {
    for(int i = 0; i < RTMC_NUM_AXES; i++) {
        scale_factors[i] = sf[i];
    }
}



void rtmc_kins_scalar_load(rtmc_path_t path) {
    // initialize the path
    scaled_path = path;

    // scale the coefficients
    switch(scaled_path.type) {
        case RTMC_PATH_TYPE_POLYNOMIAL:
            // for polynomial, scale every coefficient
            for(int i = 0; i < RTMC_NUM_AXES; i++) {
                for(int j = 0; j < RTMC_NUM_PATH_COEFFICIENTS; j++) {
                    scaled_path.coefficients[i][j] *= scale_factors[i];
                }
            }
            break;

        case RTMC_PATH_TYPE_TRIGONOMETRIC:
            // for trigonometric, scale the first and last coefficients
            for(int i = 0; i < RTMC_NUM_AXES; i++) {
                scaled_path.coefficients[i][0] *= scale_factors[i];
                scaled_path.coefficients[i][3] *= scale_factors[i];
            }
            break;
    }
}



void rtmc_kins_scalar_pose(double* pose, double s) {
    switch(scaled_path.type) {
        case RTMC_PATH_TYPE_POLYNOMIAL:
            for(int i = 0; i < RTMC_NUM_AXES; i++) {
                double A = scaled_path.coefficients[i][0];
                double B = scaled_path.coefficients[i][1];
                double C = scaled_path.coefficients[i][2];
                double D = scaled_path.coefficients[i][3];
                pose[i] = A*pow(s, 3) + B*pow(s, 2) + C*s + D;
            }
            break;
        
        case RTMC_PATH_TYPE_TRIGONOMETRIC:
            for(int i = 0; i < RTMC_NUM_AXES; i++) {
                double A = scaled_path.coefficients[i][0];
                double B = scaled_path.coefficients[i][1];
                double C = scaled_path.coefficients[i][2];
                double D = scaled_path.coefficients[i][3];
                pose[i] = A*sin(B*(s - C)) + D;
            }
            break;
    }
}
