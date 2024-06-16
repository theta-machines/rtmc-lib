/*
    parser/generate_path.c
*/

#include <math.h>
#include "parser.h"
#include "rtmc_math.h"



/*
    Determining path coefficients

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
    x(0) is the axis's start coordinate.
    x(1) is the axis's end coordinate.

    For a line in the form x(s) = As^3 + Bs^2 + Cs + D,
    A = 0
    B = 0
    C = x(1) - x(0)
    D = x(0)

    This can be derived using point-slope form of the line. Note that each
    axis has its own unique line x(s), y(s), z(s), ..., and so on. The same
    form is used for all polynomial interpolation methods.

    For trigonometric interpolation methods, x(s) = Asin(B(s - C)) + D
    is used.

    For helical interpolation, some axes are in the polynomial form, while
    others are in the trigonometric form. This relationship is implicit based
    on the selected plane. For a downward spiral (cutting into the XY plane),
    the Z-Axis would be polynomial-type (moving in a straight line), while the
    X and Y axes would be trigonometric-type (moving together in a circle).
*/

void set_line_coefficients (
    rtmc_parsed_block_t* parsed_block,
    const double* start_coords,
    const double* end_coords
    ) {
    for(int i = 0; i < RTMC_NUM_AXES; i++) {
        parsed_block->coefficients[i][0] = 0;
        parsed_block->coefficients[i][1] = 0;
        parsed_block->coefficients[i][2] = end_coords[i] - start_coords[i];
        parsed_block->coefficients[i][3] = start_coords[i];
    }
}



void generate_path(
    rtmc_parsed_block_t* parsed_block,
    const double* start_coords,
    const double* end_coords,
    const non_modal_data_t non_modal_data
    ) {

    // set is_path to false by default
    parsed_block->is_path = false;

    // handle motion mode (only if start/end coords are different)
    // TODO: this if statement only applies to arcs, not full circles!!
    //  (because start == end for full circles)
    if(!rtmc_are_vectors_equal(start_coords, end_coords, RTMC_NUM_AXES)) {

        // Rapid linear interpolation
        if(modal_data.motion_mode == G00) {
            parsed_block->is_path = true;
            parsed_block->path_type = RTMC_POLYNOMIAL;
            parsed_block->feed_rate = RTMC_RAPID_RATE;
            set_line_coefficients (parsed_block, start_coords, end_coords);
        }

        // Linear interpolation
        else if(modal_data.motion_mode == G01) {
            if(rtmc_is_greater(feed_rate, 0)) {
                parsed_block->is_path = true;
                parsed_block->path_type = RTMC_POLYNOMIAL;
                parsed_block->feed_rate = feed_rate;
                set_line_coefficients(parsed_block, start_coords, end_coords);
            }
            else {
                // invalid feed rate, invalidate the block
                parsed_block->is_valid = false;
                parsed_block->error_msg = "Feed rate is zero or negative";
            }
        }

        // Circular interpolation
        else if(modal_data.motion_mode == G02 || modal_data.motion_mode == G03) {
            // error handling
            if(modal_data.plane_mode == UNDEFINED_PLANE_MODE) {
                // invalid plane, invalidate the block
                parsed_block->is_valid = false;
                parsed_block->error_msg = "No plane selected";
            }
            else if(rtmc_is_less_equal(feed_rate, 0)) {
                // invalid feed rate, invalidate the block
                parsed_block->is_valid = false;
                parsed_block->error_msg = "Feed rate is zero or negative";
            }
            else {
                // determine plane
                int axis_0;
                int axis_1;
                switch(modal_data.plane_mode) {
                    case G17:
                        parsed_block->path_plane = RTMC_XY_PLANE;
                        axis_0 = RTMC_X_AXIS;
                        axis_1 = RTMC_Y_AXIS;
                        break;
                    
                    case G18:
                        parsed_block->path_plane = RTMC_XZ_PLANE;
                        axis_0 = RTMC_X_AXIS;
                        axis_1 = RTMC_Z_AXIS;
                        break;

                    case G19:
                        parsed_block->path_plane = RTMC_YZ_PLANE;
                        axis_0 = RTMC_Y_AXIS;
                        axis_1 = RTMC_Z_AXIS;
                        break;
                }

                // get relevant points on selected plane
                double start_point[2];
                double end_point[2];
                double offset_point[2];
                start_point[0] = start_coords[axis_0];
                start_point[1] = start_coords[axis_1];
                end_point[0] = end_coords[axis_0];
                end_point[1] = end_coords[axis_1];
                offset_point[0] = start_point[0] + non_modal_data.offset[axis_0];
                offset_point[1] = start_point[1] + non_modal_data.offset[axis_1];

                // more error handling (check if offset_point is valid)
                if(rtmc_are_vectors_equal(offset_point, start_point, 2)) {
                    parsed_block->is_valid = false;
                    parsed_block->error_msg = "Offset point cannot equal start point";
                }
                else if(rtmc_are_vectors_equal(offset_point, end_point, 2)) {
                    parsed_block->is_valid = false;
                    parsed_block->error_msg = "Offset point cannot equal end point";
                }
                else {
                    // set basic path data
                    parsed_block->is_path = true;
                    parsed_block->path_type = RTMC_TRIGONOMETRIC;
                    parsed_block->feed_rate = feed_rate;

                    bool is_clockwise = (modal_data.motion_mode == G02);

                    // find the coefficients
                    double A = rtmc_distance(start_point, offset_point, 2);
                    double B_base = acos(
                        ((offset_point[0]-start_point[0])*(offset_point[0]-end_point[0]) +
                        (offset_point[1]-start_point[1])*(offset_point[1]-end_point[1])) /
                        (A * rtmc_distance(end_point, offset_point, 2))
                    );
                    double B_sign = (
                        (offset_point[0]-start_point[0])*(offset_point[1]-end_point[1]) -
                        (offset_point[1]-start_point[1])*(offset_point[0]-end_point[0])
                    );
                    double B;
                    if(is_clockwise) {
                        B = (rtmc_is_greater_equal(B_sign, 0))
                            ? B_base - (2 * RTMC_PI)
                            : -B_base;
                    }
                    else {
                        B = (rtmc_is_greater_equal(B_sign, 0))
                            ? B_base
                            : (2 * RTMC_PI) - B_base;
                    }
                    double C_y = (
                        (1.0/B)*acos((start_point[0]-offset_point[0])/A) *
                        rtmc_sign(offset_point[1] - start_point[1])
                    );
                    double C_x = C_y - (RTMC_PI / (2*B));
                    double D_x = offset_point[0];
                    double D_y = offset_point[1];

                    // set the coefficients
                    parsed_block->coefficients[axis_0][0] = A;
                    parsed_block->coefficients[axis_0][1] = B;
                    parsed_block->coefficients[axis_0][2] = C_x;
                    parsed_block->coefficients[axis_0][3] = D_x;

                    parsed_block->coefficients[axis_1][0] = A;
                    parsed_block->coefficients[axis_1][1] = B;
                    parsed_block->coefficients[axis_1][2] = C_y;
                    parsed_block->coefficients[axis_1][3] = D_y;
                    
                    // find position_error
                    // Note: `end_coords` represents the target position
                    double actual_end_coords[12] = {0};
                    actual_end_coords[axis_0] = A*sin(B*(1-C_x)) + D_x;
                    actual_end_coords[axis_1] = A*sin(B*(1-C_y)) + D_y;
                    for(int i = 0; i < RTMC_NUM_AXES; i++) {
                        parsed_block->position_error[i] = actual_end_coords[i] - end_coords[i];
                    }
                }
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
