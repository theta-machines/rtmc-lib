#include <gtest/gtest.h>
#include "rtmc_magic_numbers.h"
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

TEST(ParseTests, G00) {
    rtmc_parsed_block_t parsed_block;
    std::string block;
    double start_coords[RTMC_NUM_AXES] = {0};

    parsed_block = rtmc_parse("G00", start_coords);
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_TRUE(parsed_block.is_path);
    EXPECT_EQ(parsed_block.path_type, RTMC_POLYNOMIAL);
    EXPECT_EQ(parsed_block.feed_rate, RTMC_RAPID_RATE);
    for(int i = 0; i < RTMC_NUM_AXES; i++) {
        EXPECT_EQ(parsed_block.coefficients[i][0], 0);
        EXPECT_EQ(parsed_block.coefficients[i][1], 0);
        EXPECT_EQ(parsed_block.coefficients[i][2], 0);
        EXPECT_EQ(parsed_block.coefficients[i][3], 0);
    }

    parsed_block = rtmc_parse("G0 X100", start_coords);
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_TRUE(parsed_block.is_path);
    EXPECT_EQ(parsed_block.path_type, RTMC_POLYNOMIAL);
    EXPECT_EQ(parsed_block.feed_rate, RTMC_RAPID_RATE);
    EXPECT_EQ(parsed_block.coefficients[RTMC_X_AXIS][0], 0);
    EXPECT_EQ(parsed_block.coefficients[RTMC_X_AXIS][1], 0);
    EXPECT_EQ(parsed_block.coefficients[RTMC_X_AXIS][2], 100);
    EXPECT_EQ(parsed_block.coefficients[RTMC_X_AXIS][3], 0);

    parsed_block = rtmc_parse("G00 X100.1 Y10.25 Z-10", start_coords);
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_TRUE(parsed_block.is_path);
    EXPECT_EQ(parsed_block.path_type, RTMC_POLYNOMIAL);
    EXPECT_EQ(parsed_block.feed_rate, RTMC_RAPID_RATE);
    EXPECT_EQ(parsed_block.coefficients[RTMC_X_AXIS][2], 100.1);
    EXPECT_EQ(parsed_block.coefficients[RTMC_Y_AXIS][2], 10.25);
    EXPECT_EQ(parsed_block.coefficients[RTMC_Z_AXIS][2], -10);

    start_coords[RTMC_Z_AXIS] = 12.5;
    parsed_block = rtmc_parse("G00 Z15", start_coords);
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_TRUE(parsed_block.is_path);
    EXPECT_EQ(parsed_block.path_type, RTMC_POLYNOMIAL);
    EXPECT_EQ(parsed_block.feed_rate, RTMC_RAPID_RATE);
    EXPECT_EQ(parsed_block.coefficients[RTMC_Z_AXIS][0], 0);
    EXPECT_EQ(parsed_block.coefficients[RTMC_Z_AXIS][1], 0);
    EXPECT_EQ(parsed_block.coefficients[RTMC_Z_AXIS][2], 15.0 - 12.5);
    EXPECT_EQ(parsed_block.coefficients[RTMC_Z_AXIS][3], 12.5);
}



/*
    Test G01 (linear) motions
    TODO: add parsed_block.is_path check
*/
TEST(ParseTests, G01) {
    rtmc_parsed_block_t parsed_block;
    std::string block;
    double start_coords[RTMC_NUM_AXES] = {0};

    // should be invalid because feed rate isn't set
    parsed_block = rtmc_parse("G01", start_coords);
    EXPECT_FALSE(parsed_block.is_valid);

    parsed_block = rtmc_parse("G01 F100", start_coords);
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_TRUE(parsed_block.is_path);
    EXPECT_EQ(parsed_block.path_type, RTMC_POLYNOMIAL);
    EXPECT_EQ(parsed_block.feed_rate, 100);
    for(int i = 0; i < RTMC_NUM_AXES; i++) {
        EXPECT_EQ(parsed_block.coefficients[i][0], 0);
        EXPECT_EQ(parsed_block.coefficients[i][1], 0);
        EXPECT_EQ(parsed_block.coefficients[i][2], 0);
        EXPECT_EQ(parsed_block.coefficients[i][3], 0);
    }

    // Should now be valid because feed rate is persistent
    parsed_block = rtmc_parse("G01", start_coords);
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_TRUE(parsed_block.is_path);
    EXPECT_EQ(parsed_block.path_type, RTMC_POLYNOMIAL);
    EXPECT_EQ(parsed_block.feed_rate, 100);
    for(int i = 0; i < RTMC_NUM_AXES; i++) {
        EXPECT_EQ(parsed_block.coefficients[i][0], 0);
        EXPECT_EQ(parsed_block.coefficients[i][1], 0);
        EXPECT_EQ(parsed_block.coefficients[i][2], 0);
        EXPECT_EQ(parsed_block.coefficients[i][3], 0);
    }

    parsed_block = rtmc_parse("G01 F150 X100.1 Y10.25 Z-10", start_coords);
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_TRUE(parsed_block.is_path);
    EXPECT_EQ(parsed_block.path_type, RTMC_POLYNOMIAL);
    EXPECT_EQ(parsed_block.feed_rate, 150);
    EXPECT_EQ(parsed_block.coefficients[RTMC_X_AXIS][2], 100.1);
    EXPECT_EQ(parsed_block.coefficients[RTMC_Y_AXIS][2], 10.25);
    EXPECT_EQ(parsed_block.coefficients[RTMC_Z_AXIS][2], -10);

    start_coords[RTMC_Z_AXIS] = 12.5;
    parsed_block = rtmc_parse("G01 Z15 F200", start_coords);
    EXPECT_TRUE(parsed_block.is_valid);
    EXPECT_TRUE(parsed_block.is_path);
    EXPECT_EQ(parsed_block.path_type, RTMC_POLYNOMIAL);
    EXPECT_EQ(parsed_block.feed_rate, 200);
    EXPECT_EQ(parsed_block.coefficients[RTMC_Z_AXIS][0], 0);
    EXPECT_EQ(parsed_block.coefficients[RTMC_Z_AXIS][1], 0);
    EXPECT_EQ(parsed_block.coefficients[RTMC_Z_AXIS][2], 15.0 - 12.5);
    EXPECT_EQ(parsed_block.coefficients[RTMC_Z_AXIS][3], 12.5);
}
