#include <gtest/gtest.h>
#include <math.h>
#include "rtmc_magic_numbers.h"
#include "rtmc_math.h"
#include "rtmc_parser.h"
#include "rtmc_parsed_block.h"

TEST(ParseTests, InvalidGrammar) {
    double start_coords[RTMC_NUM_AXES] = {0};

    EXPECT_FALSE(rtmc_parse("G", start_coords).is_valid);
    EXPECT_FALSE(rtmc_parse("1G", start_coords).is_valid);
    EXPECT_FALSE(rtmc_parse("GG0", start_coords).is_valid);
    EXPECT_FALSE(rtmc_parse("G 01", start_coords).is_valid);
    EXPECT_FALSE(rtmc_parse("G01 X100 100Y", start_coords).is_valid);
    EXPECT_FALSE(rtmc_parse("G01 X5Y10", start_coords).is_valid);
    EXPECT_FALSE(rtmc_parse("G01 X5, Y10", start_coords).is_valid);
}

TEST(ParseTests, IllegalWords) {
    double start_coords[RTMC_NUM_AXES] = {0};

    // Letters H, L, and O have no meaning
    EXPECT_FALSE(rtmc_parse("H1", start_coords).is_valid);
    EXPECT_FALSE(rtmc_parse("L1", start_coords).is_valid);
    EXPECT_FALSE(rtmc_parse("O1", start_coords).is_valid);
}

TEST(ParseTests, FlushModalData) {
    rtmc_parsed_block_t parsed_block;
    double start_coords[RTMC_NUM_AXES] = {0};

    /*
        Check that modal data is flushed by setting a feed rate, flushing it,
        then checking if the "G01 X10" is valid. If the feed rate was
        successfully flushed (reset to zero), then the test should pass.
    */
    rtmc_parse("F100", start_coords);
    rtmc_flush_modal_data();
    parsed_block = rtmc_parse("G01 X10", start_coords);
    EXPECT_FALSE(parsed_block.is_valid);
}

TEST(ParseTests, G00) {
    rtmc_parsed_block_t parsed_block;
    double start_coords[RTMC_NUM_AXES] = {0};

    parsed_block = rtmc_parse("G00", start_coords);
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_FALSE(parsed_block.is_path);

    parsed_block = rtmc_parse("G0 X100", start_coords);
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_TRUE(parsed_block.is_path);
    EXPECT_EQ(parsed_block.path_type, RTMC_POLYNOMIAL);
    EXPECT_TRUE(rtmc_is_equal(parsed_block.feed_rate, RTMC_RAPID_RATE));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_X_AXIS][0], 0));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_X_AXIS][1], 0));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_X_AXIS][2], 100));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_X_AXIS][3], 0));

    parsed_block = rtmc_parse("G00 X100.1 Y10.25 Z-10", start_coords);
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_TRUE(parsed_block.is_path);
    EXPECT_EQ(parsed_block.path_type, RTMC_POLYNOMIAL);
    EXPECT_TRUE(rtmc_is_equal(parsed_block.feed_rate, RTMC_RAPID_RATE));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_X_AXIS][2], 100.1));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_Y_AXIS][2], 10.25));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_Z_AXIS][2], -10));

    start_coords[RTMC_Z_AXIS] = 12.5;
    parsed_block = rtmc_parse("G00 Z15", start_coords);
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_TRUE(parsed_block.is_path);
    EXPECT_EQ(parsed_block.path_type, RTMC_POLYNOMIAL);
    EXPECT_TRUE(rtmc_is_equal(parsed_block.feed_rate, RTMC_RAPID_RATE));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_Z_AXIS][0], 0));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_Z_AXIS][1], 0));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_Z_AXIS][2], 15.0 - 12.5));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_Z_AXIS][3], 12.5));
}

TEST(ParseTests, G01) {
    rtmc_parsed_block_t parsed_block;
    double start_coords[RTMC_NUM_AXES] = {0};

    parsed_block = rtmc_parse("G01", start_coords);
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_FALSE(parsed_block.is_path);

    // should throw error because feed rate isn't set
    parsed_block = rtmc_parse("G01 X10", start_coords);
    EXPECT_FALSE(parsed_block.is_valid);

    parsed_block = rtmc_parse("G1 F100 X100", start_coords);
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_TRUE(parsed_block.is_path);
    EXPECT_EQ(parsed_block.path_type, RTMC_POLYNOMIAL);
    EXPECT_TRUE(rtmc_is_equal(parsed_block.feed_rate, 100));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_X_AXIS][0], 0));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_X_AXIS][1], 0));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_X_AXIS][2], 100));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_X_AXIS][3], 0));

    parsed_block = rtmc_parse("G01 F150 X100.1 Y10.25 Z-10", start_coords);
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_TRUE(parsed_block.is_path);
    EXPECT_EQ(parsed_block.path_type, RTMC_POLYNOMIAL);
    EXPECT_TRUE(rtmc_is_equal(parsed_block.feed_rate, 150));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_X_AXIS][2], 100.1));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_Y_AXIS][2], 10.25));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_Z_AXIS][2], -10));

    start_coords[RTMC_Z_AXIS] = 12.5;
    parsed_block = rtmc_parse("G01 Z15 F200", start_coords);
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_TRUE(parsed_block.is_path);
    EXPECT_EQ(parsed_block.path_type, RTMC_POLYNOMIAL);
    EXPECT_TRUE(rtmc_is_equal(parsed_block.feed_rate, 200));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_Z_AXIS][0], 0));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_Z_AXIS][1], 0));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_Z_AXIS][2], 15.0 - 12.5));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_Z_AXIS][3], 12.5));
}

TEST(ParseTests, G02_IJK_Arcs_Syntax) {
    rtmc_parsed_block_t parsed_block;
    double start_coords[RTMC_NUM_AXES] = {0};

    // valid - just setting modal data (not a path though!)
    parsed_block = rtmc_parse("G02", start_coords);
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_FALSE(parsed_block.is_path);

    // invalid - no plane is selected
    parsed_block = rtmc_parse("G02 X100 Y250 I100 J100", start_coords);
    EXPECT_FALSE(parsed_block.is_valid);

    // invalid - no feed rate set
    parsed_block = rtmc_parse("G17 G02 X100 Y250 I100 J100", start_coords);
    EXPECT_FALSE(parsed_block.is_valid);

    // invalid - missing an offset (I and J words)
    parsed_block = rtmc_parse("G17 G02 F100 X100 Y250", start_coords);
    EXPECT_FALSE(parsed_block.is_valid);
}

TEST(ParseTests, G02_IJK_Arcs_Coefficients) {
    rtmc_parsed_block_t parsed_block;
    double start_coords[RTMC_NUM_AXES] = {0};
    
    start_coords[0] = -100;
    start_coords[1] = -50;
    parsed_block = rtmc_parse("G17 G02 F100 X100 Y250 I100 J100", start_coords);
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_TRUE(parsed_block.is_path);
    EXPECT_EQ(parsed_block.path_type, RTMC_TRIGONOMETRIC);

    double A = sqrt(2e4);
    double B = -acos(-3e4 / sqrt(1e9));
    double C_y = (3*RTMC_PI) / (4*B);
    double C_x = RTMC_PI / (4*B);
    double D_x = 0;
    double D_y = 50;
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_X_AXIS][0], A));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_X_AXIS][1], B));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_X_AXIS][2], C_x));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_X_AXIS][3], D_x));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_Y_AXIS][0], A));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_Y_AXIS][1], B));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_Y_AXIS][2], C_y));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_Y_AXIS][3], D_y));

    double true_end_pos[12] = {0};
    true_end_pos[RTMC_X_AXIS] = A*sin(B*(1-C_x)) + D_x;
    true_end_pos[RTMC_Y_AXIS] = A*sin(B*(1-C_y)) + D_y;

    double target_position[12] = {0};
    target_position[RTMC_X_AXIS] = 100;
    target_position[RTMC_Y_AXIS] = 250;

    double position_error[12] = {0};
    position_error[RTMC_X_AXIS] = true_end_pos[RTMC_X_AXIS] - target_position[RTMC_X_AXIS];
    position_error[RTMC_Y_AXIS] = true_end_pos[RTMC_Y_AXIS] - target_position[RTMC_Y_AXIS];

    // actually check that the position error is correct
    for(int i = 2; i < RTMC_NUM_AXES; i++) {
        EXPECT_TRUE(rtmc_is_equal(position_error[i], parsed_block.position_error[i]));
    }
}

TEST(ParseTests, G03_IJK_Arcs_Syntax) {
    rtmc_parsed_block_t parsed_block;
    double start_coords[RTMC_NUM_AXES] = {0};

    // valid - just setting modal data (not a path though!)
    parsed_block = rtmc_parse("G03", start_coords);
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_FALSE(parsed_block.is_path);

    // invalid - no plane is selected
    parsed_block = rtmc_parse("G03 X100 Y250 I100 J100", start_coords);
    EXPECT_FALSE(parsed_block.is_valid);

    // invalid - no feed rate set
    parsed_block = rtmc_parse("G17 G03 X100 Y250 I100 J100", start_coords);
    EXPECT_FALSE(parsed_block.is_valid);

    // invalid - missing an offset (I and J words)
    parsed_block = rtmc_parse("G17 G03 F100 X100 Y250", start_coords);
    EXPECT_FALSE(parsed_block.is_valid);
}

TEST(ParseTests, G03_IJK_Arcs_Coefficients) {
    rtmc_parsed_block_t parsed_block;
    double start_coords[RTMC_NUM_AXES] = {0};
    
    start_coords[0] = -100;
    start_coords[1] = -50;
    parsed_block = rtmc_parse("G17 G03 F100 X100 Y250 I100 J100", start_coords);
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_TRUE(parsed_block.is_path);
    EXPECT_EQ(parsed_block.path_type, RTMC_TRIGONOMETRIC);

    double A = sqrt(2e4);
    double B = 2*RTMC_PI - acos(-3e4 / sqrt(1e9));
    double C_y = (3*RTMC_PI) / (4*B);
    double C_x = RTMC_PI / (4*B);
    double D_x = 0;
    double D_y = 50;
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_X_AXIS][0], A));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_X_AXIS][1], B));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_X_AXIS][2], C_x));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_X_AXIS][3], D_x));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_Y_AXIS][0], A));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_Y_AXIS][1], B));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_Y_AXIS][2], C_y));
    EXPECT_TRUE(rtmc_is_equal(parsed_block.coefficients[RTMC_Y_AXIS][3], D_y));

    double true_end_pos[12] = {0};
    true_end_pos[RTMC_X_AXIS] = A*sin(B*(1-C_x)) + D_x;
    true_end_pos[RTMC_Y_AXIS] = A*sin(B*(1-C_y)) + D_y;

    double target_position[12] = {0};
    target_position[RTMC_X_AXIS] = 100;
    target_position[RTMC_Y_AXIS] = 250;

    double position_error[12] = {0};
    position_error[RTMC_X_AXIS] = true_end_pos[RTMC_X_AXIS] - target_position[RTMC_X_AXIS];
    position_error[RTMC_Y_AXIS] = true_end_pos[RTMC_Y_AXIS] - target_position[RTMC_Y_AXIS];

    // actually check that the position error is correct
    for(int i = 2; i < RTMC_NUM_AXES; i++) {
        EXPECT_TRUE(rtmc_is_equal(position_error[i], parsed_block.position_error[i]));
    }
}

TEST(ParseTests, G17) {
    // XY plane is default, so a different plane must be selected first
    double start_coords[RTMC_NUM_AXES] = {0};
    rtmc_parsed_block_t parsed_block;

    // set plane
    rtmc_parse("G18", start_coords);

    // change plane back
    parsed_block = rtmc_parse("G17", start_coords);
    EXPECT_TRUE(parsed_block.is_valid);
}

TEST(ParseTests, G18) {
    double start_coords[RTMC_NUM_AXES] = {0};
    rtmc_parsed_block_t parsed_block = rtmc_parse("G18", start_coords);
    EXPECT_TRUE(parsed_block.is_valid);
}

TEST(ParseTests, G19) {
    double start_coords[RTMC_NUM_AXES] = {0};
    rtmc_parsed_block_t parsed_block = rtmc_parse("G19", start_coords);
    EXPECT_TRUE(parsed_block.is_valid);
}
