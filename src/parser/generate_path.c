/*
    parser/generate_path.c
*/

#include <math.h>
#include "parser.h"
#include "rtmc_math.h"



/*
    This function generates path data and adds it to the `parsed_block`.



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

    For trigonometric interpolation methods, x(s) = Asin(B(s - C)) + D
    is used.

    For helical interpolation, some axes are in the polynomial form, while
    others are in the trigonometric form. This relationship is implicit based
    on the selected plane. For a downward spiral (cutting into the XY plane),
    the Z-Axis would be polynomial-type (moving in a straight line), while the
    X and Y axes would be trigonometric-type (moving together in a circle).
*/
void generate_path(
    rtmc_parsed_block_t* parsed_block,
    const double* start_coords,
    const double* end_coords,
    const non_modal_data_t non_modal_data
    ) {

    // set is_path to true by default
    parsed_block->is_path = false;



    // handle motion mode (only if start/end coords are different)
    if(!rtmc_are_vectors_equal(start_coords, end_coords, RTMC_NUM_AXES)) {

        // Rapid linear interpolation
        if(modal_data.motion_mode == G00) {
            parsed_block->is_path = true;
            parsed_block->path_type = RTMC_POLYNOMIAL;
            parsed_block->feed_rate = RTMC_RAPID_RATE;

            // set coefficients (polynomial, linear)
            for(int i = 0; i < RTMC_NUM_AXES; i++) {
                parsed_block->coefficients[i][0] = 0;
                parsed_block->coefficients[i][1] = 0;
                parsed_block->coefficients[i][2] = end_coords[i] - start_coords[i];
                parsed_block->coefficients[i][3] = start_coords[i];
            }
        }

        // Linear interpolation
        else if(modal_data.motion_mode == G01) {
            if(feed_rate > 0) {
                parsed_block->is_path = true;
                parsed_block->path_type = RTMC_POLYNOMIAL;
                parsed_block->feed_rate = feed_rate;

                // set coefficients (polynomial, linear)
                for(int i = 0; i < RTMC_NUM_AXES; i++) {
                    parsed_block->coefficients[i][0] = 0;
                    parsed_block->coefficients[i][1] = 0;
                    parsed_block->coefficients[i][2] = end_coords[i] - start_coords[i];
                    parsed_block->coefficients[i][3] = start_coords[i];
                }   
            }
            else {
                // invalid feed rate, invalidate the block
                parsed_block->is_valid = false;
                parsed_block->error_msg = "Feed rate is zero or negative";
            }
        }
    }



    // handle plane mode
    if(modal_data.plane_mode == G17) {
        parsed_block->path_plane = RTMC_XY_PLANE;
    }
    else if(modal_data.plane_mode == G18) {
        parsed_block->path_plane = RTMC_XZ_PLANE;
    }
    else if(modal_data.plane_mode == G19) {
        parsed_block->path_plane = RTMC_YZ_PLANE;
    }
}
