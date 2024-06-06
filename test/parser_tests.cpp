#include <gtest/gtest.h>
#include "rtmc_magic_numbers.h"
#include "rtmc_parser.h"
#include "rtmc_path.h"

/*
    Test that the parsing FSM properly rejects g-code blocks that do not
    consist of valid key/value pairs (a.k.a, words)
*/
TEST(ParseTests, GrammarErrors) {
    rtmc_path_t path;
    char block[256];
    double start_coords[RTMC_NUM_AXES] = {0};

    strcpy(block, "G");
    rtmc_parse(block, start_coords, &path);
    EXPECT_FALSE(path.is_valid);

    strcpy(block, "1G");
    rtmc_parse(block, start_coords, &path);
    EXPECT_FALSE(path.is_valid);

    strcpy(block, "G01 X100 100Y");
    rtmc_parse(block, start_coords, &path);
    EXPECT_FALSE(path.is_valid);

    strcpy(block, "GG0");
    rtmc_parse(block, start_coords, &path);
    EXPECT_FALSE(path.is_valid);

    strcpy(block, "G01 X5K");
    rtmc_parse(block, start_coords, &path);
    EXPECT_FALSE(path.is_valid);

    strcpy(block, "G01 X100 & Y100");
    rtmc_parse(block, start_coords, &path);
    EXPECT_FALSE(path.is_valid);

    strcpy(block, "G01 X100, Y100");
    rtmc_parse(block, start_coords, &path);
    EXPECT_FALSE(path.is_valid);

    strcpy(block, "G01 X 100");
    rtmc_parse(block, start_coords, &path);
    EXPECT_FALSE(path.is_valid);
}



/*
    Test that invalid key/value pairs (a.k.a. words) are rejected
*/
TEST(ParseTests, InvalidWordErrors) {
    rtmc_path_t path;
    char block[256];
    double start_coords[RTMC_NUM_AXES] = {0};

    // Letter H unassigned
    strcpy(block, "G01 H1");
    rtmc_parse(block, start_coords, &path);
    EXPECT_FALSE(path.is_valid);

    // Letter L unassigned
    strcpy(block, "G01 L1");
    rtmc_parse(block, start_coords, &path);
    EXPECT_FALSE(path.is_valid);

    // Letter O unassigned
    strcpy(block, "G01 O1");
    rtmc_parse(block, start_coords, &path);
    EXPECT_FALSE(path.is_valid);
}



/*
    Test G00 (rapid) motions
*/
TEST(ParseTests, G00) {
    rtmc_path_t path;
    char block[256];
    double start_coords[RTMC_NUM_AXES] = {0};

    strcpy(block, "G00");
    rtmc_parse(block, start_coords, &path);
    EXPECT_TRUE(path.is_valid);
    EXPECT_EQ(path.feed_rate, RTMC_RAPID_RATE);
    EXPECT_EQ(path.type, RTMC_POLYNOMIAL);
    for(int i = 0; i < RTMC_NUM_AXES; i++) {
        EXPECT_EQ(path.coefficients[i][0], 0);
        EXPECT_EQ(path.coefficients[i][1], 0);
        EXPECT_EQ(path.coefficients[i][2], 0);
        EXPECT_EQ(path.coefficients[i][3], 0);
    }

    strcpy(block, "G00 X100");
    rtmc_parse(block, start_coords, &path);
    EXPECT_TRUE(path.is_valid);
    EXPECT_EQ(path.feed_rate, RTMC_RAPID_RATE);
    EXPECT_EQ(path.type, RTMC_POLYNOMIAL);
    EXPECT_EQ(path.coefficients[RTMC_X_AXIS][0], 0);
    EXPECT_EQ(path.coefficients[RTMC_X_AXIS][1], 0);
    EXPECT_EQ(path.coefficients[RTMC_X_AXIS][2], 100);
    EXPECT_EQ(path.coefficients[RTMC_X_AXIS][3], 0);

    strcpy(block, "G00 Q-1E-5");
    rtmc_parse(block, start_coords, &path);
    EXPECT_TRUE(path.is_valid);
    EXPECT_EQ(path.feed_rate, RTMC_RAPID_RATE);
    EXPECT_EQ(path.type, RTMC_POLYNOMIAL);
    EXPECT_EQ(path.coefficients[RTMC_Q_AXIS][0], 0);
    EXPECT_EQ(path.coefficients[RTMC_Q_AXIS][1], 0);
    EXPECT_EQ(path.coefficients[RTMC_Q_AXIS][2], -1E-5);
    EXPECT_EQ(path.coefficients[RTMC_Q_AXIS][3], 0);

    strcpy(block, "G00 P1e5");
    rtmc_parse(block, start_coords, &path);
    EXPECT_TRUE(path.is_valid);
    EXPECT_EQ(path.feed_rate, RTMC_RAPID_RATE);
    EXPECT_EQ(path.type, RTMC_POLYNOMIAL);
    EXPECT_EQ(path.coefficients[RTMC_P_AXIS][0], 0);
    EXPECT_EQ(path.coefficients[RTMC_P_AXIS][1], 0);
    EXPECT_EQ(path.coefficients[RTMC_P_AXIS][2], 1E5);
    EXPECT_EQ(path.coefficients[RTMC_P_AXIS][3], 0);

    strcpy(block, "G00 X100.1 Y10.25 Z-10");
    rtmc_parse(block, start_coords, &path);
    EXPECT_TRUE(path.is_valid);
    EXPECT_EQ(path.feed_rate, RTMC_RAPID_RATE);
    EXPECT_EQ(path.type, RTMC_POLYNOMIAL);
    EXPECT_EQ(path.coefficients[RTMC_X_AXIS][2], 100.1);
    EXPECT_EQ(path.coefficients[RTMC_Y_AXIS][2], 10.25);
    EXPECT_EQ(path.coefficients[RTMC_Z_AXIS][2], -10);

    start_coords[RTMC_Z_AXIS] = 12.5;
    strcpy(block, "G00 Z15");
    rtmc_parse(block, start_coords, &path);
    EXPECT_TRUE(path.is_valid);
    EXPECT_EQ(path.feed_rate, RTMC_RAPID_RATE);
    EXPECT_EQ(path.type, RTMC_POLYNOMIAL);
    EXPECT_EQ(path.coefficients[RTMC_Z_AXIS][0], 0);
    EXPECT_EQ(path.coefficients[RTMC_Z_AXIS][1], 0);
    EXPECT_EQ(path.coefficients[RTMC_Z_AXIS][2], 15.0 - 12.5);
    EXPECT_EQ(path.coefficients[RTMC_Z_AXIS][3], 12.5);
}



/*
    Test G01 (linear) motions
*/
TEST(ParseTests, G01) {
    rtmc_path_t path;
    char block[256];
    double start_coords[RTMC_NUM_AXES] = {0};

    // should be invalid because feed rate isn't set
    strcpy(block, "G01");
    rtmc_parse(block, start_coords, &path);
    EXPECT_FALSE(path.is_valid);

    strcpy(block, "G01 F1.0E2");
    rtmc_parse(block, start_coords, &path);
    EXPECT_TRUE(path.is_valid);
    EXPECT_EQ(path.feed_rate, 1.0E2);
    EXPECT_EQ(path.type, RTMC_POLYNOMIAL);
    for(int i = 0; i < RTMC_NUM_AXES; i++) {
        EXPECT_EQ(path.coefficients[i][0], 0);
        EXPECT_EQ(path.coefficients[i][1], 0);
        EXPECT_EQ(path.coefficients[i][2], 0);
        EXPECT_EQ(path.coefficients[i][3], 0);
    }

    // Should now be valid because feed rate is persistent
    strcpy(block, "G01");
    rtmc_parse(block, start_coords, &path);
    EXPECT_TRUE(path.is_valid);
    EXPECT_EQ(path.feed_rate, 1.0E2);
    EXPECT_EQ(path.type, RTMC_POLYNOMIAL);
    for(int i = 0; i < RTMC_NUM_AXES; i++) {
        EXPECT_EQ(path.coefficients[i][0], 0);
        EXPECT_EQ(path.coefficients[i][1], 0);
        EXPECT_EQ(path.coefficients[i][2], 0);
        EXPECT_EQ(path.coefficients[i][3], 0);
    }

    strcpy(block, "G01 F100 X100");
    rtmc_parse(block, start_coords, &path);
    EXPECT_TRUE(path.is_valid);
    EXPECT_EQ(path.feed_rate, 100);
    EXPECT_EQ(path.type, RTMC_POLYNOMIAL);
    EXPECT_EQ(path.coefficients[RTMC_X_AXIS][0], 0);
    EXPECT_EQ(path.coefficients[RTMC_X_AXIS][1], 0);
    EXPECT_EQ(path.coefficients[RTMC_X_AXIS][2], 100);
    EXPECT_EQ(path.coefficients[RTMC_X_AXIS][3], 0);

    start_coords[RTMC_Z_AXIS] = 12.5;
    strcpy(block, "G01 F100 X100 Z100.0");
    rtmc_parse(block, start_coords, &path);
    EXPECT_TRUE(path.is_valid);
    EXPECT_EQ(path.feed_rate, 100);
    EXPECT_EQ(path.type, RTMC_POLYNOMIAL);
    EXPECT_EQ(path.coefficients[RTMC_Z_AXIS][0], 0);
    EXPECT_EQ(path.coefficients[RTMC_Z_AXIS][1], 0);
    EXPECT_EQ(path.coefficients[RTMC_Z_AXIS][2], 100 - 12.5);
    EXPECT_EQ(path.coefficients[RTMC_Z_AXIS][3], 12.5);

}
