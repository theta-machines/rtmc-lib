#include <gtest/gtest.h>
#include <math.h>
#include "rtmc_magic_numbers.h"
#include "rtmc_math.h"
#include "rtmc_parser.h"
#include "rtmc_path.h"

TEST(ParseTests, InvalidGrammar) {
    rtmc_path_queue_t queue = rtmc_create_path_queue();

    EXPECT_FALSE(rtmc_parse(&queue, "G").is_valid);
    EXPECT_FALSE(rtmc_parse(&queue, "1G").is_valid);
    EXPECT_FALSE(rtmc_parse(&queue, "GG0").is_valid);
    EXPECT_FALSE(rtmc_parse(&queue, "G 01").is_valid);
    EXPECT_FALSE(rtmc_parse(&queue, "G01 X100 100Y").is_valid);
    EXPECT_FALSE(rtmc_parse(&queue, "G01 X5Y10").is_valid);
    EXPECT_FALSE(rtmc_parse(&queue, "G01 X5, Y10").is_valid);
}

TEST(ParseTests, IllegalWords) {
    rtmc_path_queue_t queue = rtmc_create_path_queue();

    // Letters H, L, and O have no meaning
    EXPECT_FALSE(rtmc_parse(&queue, "H1").is_valid);
    EXPECT_FALSE(rtmc_parse(&queue, "L1").is_valid);
    EXPECT_FALSE(rtmc_parse(&queue, "O1").is_valid);
}

TEST(ParseTests, ValueTooLong) {
    rtmc_path_queue_t queue = rtmc_create_path_queue();

    EXPECT_FALSE(rtmc_parse(&queue, "G0 X-1.4273956103948e-13").is_valid);
}

TEST(ParseTests, FlushModalData) {
    rtmc_path_queue_t queue = rtmc_create_path_queue();

    /*
        Check that modal data is flushed by setting a feed rate, flushing it,
        then checking if the "G01 X10" is valid. If the feed rate was
        successfully flushed (reset to zero), then the test should pass.
    */
    rtmc_parse(&queue, "F100");
    rtmc_flush_parser_data();
    EXPECT_FALSE(rtmc_parse(&queue, "G01 X10").is_valid);
}

TEST(ParseTests, G00) {
    rtmc_path_queue_t queue = rtmc_create_path_queue();
    rtmc_parsed_block_t parsed_block;
    rtmc_path_t path;

    parsed_block = rtmc_parse(&queue, "G00");
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_EQ(parsed_block.type, RTMC_BLOCK_TYPE_MODAL);

    parsed_block = rtmc_parse(&queue, "G0 X100");
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_EQ(parsed_block.type, RTMC_BLOCK_TYPE_PATH);
    path = rtmc_path_dequeue(&queue);
    EXPECT_EQ(path.type, RTMC_PATH_TYPE_POLYNOMIAL);
    EXPECT_TRUE(rtmc_is_equal(path.feed_rate, RTMC_RAPID_RATE));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][0], 0));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][1], 0));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][2], 100));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][3], 0));

    parsed_block = rtmc_parse(&queue, "G00 X100.1 Y10.25 Z-10");
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_EQ(parsed_block.type, RTMC_BLOCK_TYPE_PATH);
    path = rtmc_path_dequeue(&queue);
    EXPECT_EQ(path.type, RTMC_PATH_TYPE_POLYNOMIAL);
    EXPECT_TRUE(rtmc_is_equal(path.feed_rate, RTMC_RAPID_RATE));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][2], 0.1));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Y_AXIS][2], 10.25));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Z_AXIS][2], -10));

    parsed_block = rtmc_parse(&queue, "G00 Z15");
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_EQ(parsed_block.type, RTMC_BLOCK_TYPE_PATH);
    path = rtmc_path_dequeue(&queue);
    EXPECT_EQ(path.type, RTMC_PATH_TYPE_POLYNOMIAL);
    EXPECT_TRUE(rtmc_is_equal(path.feed_rate, RTMC_RAPID_RATE));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Z_AXIS][0], 0));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Z_AXIS][1], 0));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Z_AXIS][2], 15.0 - -10));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Z_AXIS][3], -10));
}

TEST(ParseTests, G01) {
    rtmc_path_queue_t queue = rtmc_create_path_queue();
    rtmc_parsed_block_t parsed_block;
    rtmc_path_t path;

    parsed_block = rtmc_parse(&queue, "G01");
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_EQ(parsed_block.type, RTMC_BLOCK_TYPE_MODAL);

    // should throw error because feed rate isn't set
    parsed_block = rtmc_parse(&queue, "G01 X10");
    EXPECT_FALSE(parsed_block.is_valid);

    rtmc_flush_parser_data();
    parsed_block = rtmc_parse(&queue, "G1 F100 X100");
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_EQ(parsed_block.type, RTMC_BLOCK_TYPE_PATH);
    path = rtmc_path_dequeue(&queue);
    EXPECT_EQ(path.type, RTMC_PATH_TYPE_POLYNOMIAL);
    EXPECT_TRUE(rtmc_is_equal(path.feed_rate, 100));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][0], 0));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][1], 0));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][2], 100));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][3], 0));

    rtmc_flush_parser_data();
    parsed_block = rtmc_parse(&queue, "G01 F150 X100.1 Y10.25 Z-10");
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_EQ(parsed_block.type, RTMC_BLOCK_TYPE_PATH);
    path = rtmc_path_dequeue(&queue);
    EXPECT_EQ(path.type, RTMC_PATH_TYPE_POLYNOMIAL);
    EXPECT_TRUE(rtmc_is_equal(path.feed_rate, 150));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][2], 100.1));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Y_AXIS][2], 10.25));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Z_AXIS][2], -10));

    parsed_block = rtmc_parse(&queue, "G01 Z15 F200");
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_EQ(parsed_block.type, RTMC_BLOCK_TYPE_PATH);
    path = rtmc_path_dequeue(&queue);
    EXPECT_EQ(path.type, RTMC_PATH_TYPE_POLYNOMIAL);
    EXPECT_TRUE(rtmc_is_equal(path.feed_rate, 200));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Z_AXIS][0], 0));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Z_AXIS][1], 0));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Z_AXIS][2], 15.0 - -10));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Z_AXIS][3], -10));
}

TEST(ParseTests, G02_IJK_Syntax) {
    rtmc_path_queue_t queue = rtmc_create_path_queue();
    rtmc_parsed_block_t parsed_block;

    // invalid - needs complimentary parameters
    parsed_block = rtmc_parse(&queue, "G02");
    EXPECT_FALSE(parsed_block.is_valid);

    // invalid - no plane is selected
    parsed_block = rtmc_parse(&queue, "G02 X100 Y250 I100 J100");
    EXPECT_FALSE(parsed_block.is_valid);

    // invalid - no feed rate set
    parsed_block = rtmc_parse(&queue, "G17 G02 X100 Y250 I100 J100");
    EXPECT_FALSE(parsed_block.is_valid);

    // // invalid - missing an offset (I and J words)
    parsed_block = rtmc_parse(&queue, "G17 G02 F100 X100 Y250");
    EXPECT_FALSE(parsed_block.is_valid);
}

TEST(ParseTests, G02_IJK_ArcPath) {
    rtmc_path_queue_t queue = rtmc_create_path_queue();
    rtmc_parsed_block_t parsed_block;
    rtmc_path_t path;

    // set start coordinates
    rtmc_parse(&queue, "G00 X-100 Y-50");
    rtmc_path_dequeue(&queue);
    
    parsed_block = rtmc_parse(&queue, "G17 G02 F100 X100 Y250 I100 J100");
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_EQ(parsed_block.type, RTMC_BLOCK_TYPE_PATH);
    path = rtmc_path_dequeue(&queue);
    EXPECT_EQ(path.type, RTMC_PATH_TYPE_TRIGONOMETRIC);

    double A = sqrt(2e4);
    double B = -acos(-3e4 / sqrt(1e9));
    double C_y = (3*RTMC_PI) / (4*B);
    double C_x = RTMC_PI / (4*B);
    double D_x = 0;
    double D_y = 50;
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][0], A));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][1], B));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][2], C_x));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][3], D_x));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Y_AXIS][0], A));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Y_AXIS][1], B));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Y_AXIS][2], C_y));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Y_AXIS][3], D_y));

    // find position error
    double true_end_pos[12] = {0};
    true_end_pos[RTMC_X_AXIS] = A*sin(B*(1-C_x)) + D_x;
    true_end_pos[RTMC_Y_AXIS] = A*sin(B*(1-C_y)) + D_y;

    double target_position[12] = {0};
    target_position[RTMC_X_AXIS] = 100;
    target_position[RTMC_Y_AXIS] = 250;

    double position_error[12] = {0};
    position_error[RTMC_X_AXIS] = true_end_pos[RTMC_X_AXIS] - target_position[RTMC_X_AXIS];
    position_error[RTMC_Y_AXIS] = true_end_pos[RTMC_Y_AXIS] - target_position[RTMC_Y_AXIS];

    // check that the position error is correct
    for(int i = 2; i < RTMC_NUM_AXES; i++) {
        EXPECT_TRUE(rtmc_is_equal(position_error[i], parsed_block.position_error[i]));
    }

    // check that `true_end_pos` is what is used for `end_coords` internally
    // (do this by checking the last coefficient of the linear move)
    parsed_block = rtmc_parse(&queue, "G00 X0 Y0");
    path = rtmc_path_dequeue(&queue);
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][3], true_end_pos[RTMC_X_AXIS]));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Y_AXIS][3], true_end_pos[RTMC_Y_AXIS]));
}

TEST(ParseTests, G02_FullCircle) {
    rtmc_path_queue_t queue = rtmc_create_path_queue();
    rtmc_parsed_block_t parsed_block;
    rtmc_path_t path;
    
    // set start coordinates
    rtmc_parse(&queue, "G00 X-100 Y-50");
    rtmc_path_dequeue(&queue);

    parsed_block = rtmc_parse(&queue, "G17 G02 F100 I100 J100");
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_EQ(parsed_block.type, RTMC_BLOCK_TYPE_PATH);
    path = rtmc_path_dequeue(&queue);
    EXPECT_EQ(path.type, RTMC_PATH_TYPE_TRIGONOMETRIC);

    double A = sqrt(2e4);
    double B = -2.0*RTMC_PI;
    double C_y = -3.0 / 8.0;
    double C_x = -1.0 / 8.0;
    double D_x = 0;
    double D_y = 50;
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][0], A));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][1], B));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][2], C_x));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][3], D_x));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Y_AXIS][0], A));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Y_AXIS][1], B));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Y_AXIS][2], C_y));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Y_AXIS][3], D_y));

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

TEST(ParseTests, G03_IJK_Syntax) {
    rtmc_path_queue_t queue = rtmc_create_path_queue();
    rtmc_parsed_block_t parsed_block;

    // invalid - needs complimentary parameters
    parsed_block = rtmc_parse(&queue, "G03");
    EXPECT_FALSE(parsed_block.is_valid);

    // invalid - no plane is selected
    parsed_block = rtmc_parse(&queue, "G03 X100 Y250 I100 J100");
    EXPECT_FALSE(parsed_block.is_valid);

    // invalid - no feed rate set
    parsed_block = rtmc_parse(&queue, "G17 G03 X100 Y250 I100 J100");
    EXPECT_FALSE(parsed_block.is_valid);

    // // invalid - missing an offset (I and J words)
    parsed_block = rtmc_parse(&queue, "G17 G03 F100 X100 Y250");
    EXPECT_FALSE(parsed_block.is_valid);
}

TEST(ParseTests, G03_IJK_ArcPath) {
    rtmc_path_queue_t queue = rtmc_create_path_queue();
    rtmc_parsed_block_t parsed_block;
    rtmc_path_t path;

    // set start coordinates
    rtmc_parse(&queue, "G00 X-100 Y-50");
    rtmc_path_dequeue(&queue);
    
    parsed_block = rtmc_parse(&queue, "G17 G03 F100 X100 Y250 I100 J100");
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_EQ(parsed_block.type, RTMC_BLOCK_TYPE_PATH);
    path = rtmc_path_dequeue(&queue);
    EXPECT_EQ(path.type, RTMC_PATH_TYPE_TRIGONOMETRIC);

    double A = sqrt(2e4);
    double B = 2*RTMC_PI - acos(-3e4 / sqrt(1e9));
    double C_y = (3*RTMC_PI) / (4*B);
    double C_x = RTMC_PI / (4*B);
    double D_x = 0;
    double D_y = 50;
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][0], A));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][1], B));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][2], C_x));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][3], D_x));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Y_AXIS][0], A));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Y_AXIS][1], B));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Y_AXIS][2], C_y));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Y_AXIS][3], D_y));

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

    // check that `true_end_pos` is what is used for `end_coords` internally
    // (do this by checking the last coefficient of the linear move)
    parsed_block = rtmc_parse(&queue, "G00 X0 Y0");
    path = rtmc_path_dequeue(&queue);
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][3], true_end_pos[RTMC_X_AXIS]));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Y_AXIS][3], true_end_pos[RTMC_Y_AXIS]));
}

TEST(ParseTests, G03_FullCircle) {
    rtmc_path_queue_t queue = rtmc_create_path_queue();
    rtmc_parsed_block_t parsed_block;
    rtmc_path_t path;
    
    // set start coordinates
    rtmc_parse(&queue, "G00 X-100 Y-50");
    rtmc_path_dequeue(&queue);

    parsed_block = rtmc_parse(&queue, "G17 G03 F100 I100 J100");
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_EQ(parsed_block.type, RTMC_BLOCK_TYPE_PATH);
    path = rtmc_path_dequeue(&queue);
    EXPECT_EQ(path.type, RTMC_PATH_TYPE_TRIGONOMETRIC);

    double A = sqrt(2e4);
    double B = 2.0*RTMC_PI;
    double C_y = 3.0 / 8.0;
    double C_x = 1.0 / 8.0;
    double D_x = 0;
    double D_y = 50;
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][0], A));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][1], B));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][2], C_x));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_X_AXIS][3], D_x));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Y_AXIS][0], A));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Y_AXIS][1], B));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Y_AXIS][2], C_y));
    EXPECT_TRUE(rtmc_is_equal(path.coefficients[RTMC_Y_AXIS][3], D_y));

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

// TODO: test G02/G03 with other planes (G18 and G19)

TEST(ParseTests, G17) {
    rtmc_path_queue_t queue = rtmc_create_path_queue();
    rtmc_parsed_block_t parsed_block;
    rtmc_path_t path;

    // XY plane is default, so a different plane must be selected first
    rtmc_parse(&queue, "G18");
    rtmc_path_dequeue(&queue);

    // change plane back
    EXPECT_TRUE(rtmc_parse(&queue, "G17").is_valid);
}

TEST(ParseTests, G18) {
    rtmc_path_queue_t queue = rtmc_create_path_queue();
    EXPECT_TRUE(rtmc_parse(&queue, "G18").is_valid);
}

TEST(ParseTests, G19) {
    rtmc_path_queue_t queue = rtmc_create_path_queue();
    EXPECT_TRUE(rtmc_parse(&queue, "G19").is_valid);
}
