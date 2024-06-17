#include <float.h>
#include <math.h>
#include <gtest/gtest.h>
#include "rtmc_math.h"

/*
    Double Comparison Tests
*/
TEST(MathTests, IsEqual_TypicalNumbers) {
    EXPECT_TRUE(rtmc_is_equal(1.0, 1.0));
    EXPECT_TRUE(rtmc_is_equal(-0.0, 0.0));
    EXPECT_TRUE(rtmc_is_equal(1.0 / sqrt(2), sqrt(2) / 2));
    EXPECT_FALSE(rtmc_is_equal(1.0, 2.0));
    EXPECT_FALSE(rtmc_is_equal(2.0, 1.0));
}

TEST(MathTests, IsEqual_ThresholdBoundaries) {
    // barely within threshold
    double difference = 99 * DBL_MIN;
    EXPECT_TRUE(rtmc_is_equal(1.0, 1.0 + difference));
    EXPECT_TRUE(rtmc_is_equal(1.0, 1.0 - difference));
    EXPECT_TRUE(rtmc_is_equal(1.0 + difference, 1.0));
    EXPECT_TRUE(rtmc_is_equal(1.0 - difference, 1.0));

    // barely outside of threshold
    difference = 201 * DBL_EPSILON;
    EXPECT_FALSE(rtmc_is_equal(1.0, 1.0 + difference));
    EXPECT_FALSE(rtmc_is_equal(1.0, 1.0 - difference));
    EXPECT_FALSE(rtmc_is_equal(1.0 + difference, 1.0));
    EXPECT_FALSE(rtmc_is_equal(1.0 - difference, 1.0));
}

TEST(MathTests, IsEqual_ExtremeNumbers) {
    // small numbers
    double small_number = DBL_MIN * 99;
    EXPECT_TRUE(rtmc_is_equal(DBL_MIN, small_number));
    EXPECT_TRUE(rtmc_is_equal(0, small_number));

    // large numbers
    double large_number = DBL_MAX * (1 - 1e-10);
    EXPECT_TRUE(rtmc_is_equal(DBL_MAX, large_number));

    // min/max boundary
    EXPECT_TRUE(rtmc_is_equal(DBL_MAX, DBL_MAX));
    EXPECT_TRUE(rtmc_is_equal(DBL_MIN, DBL_MIN));
}

TEST(MathTests, IsEqual_NonNumbers) {
    EXPECT_TRUE(rtmc_is_equal(INFINITY, INFINITY));
    EXPECT_FALSE(rtmc_is_equal(INFINITY, -INFINITY));
    EXPECT_FALSE(rtmc_is_equal(INFINITY, 0.0));
    EXPECT_FALSE(rtmc_is_equal(INFINITY, DBL_MAX));
    EXPECT_FALSE(rtmc_is_equal(-INFINITY, DBL_MIN));
    EXPECT_FALSE(rtmc_is_equal(NAN, NAN));
    EXPECT_FALSE(rtmc_is_equal(NAN, INFINITY));
    EXPECT_FALSE(rtmc_is_equal(NAN, 0.0));
}

TEST(MathTests, IsGreater) {
    EXPECT_TRUE(rtmc_is_greater(2.0, 1.0));
    EXPECT_FALSE(rtmc_is_greater(1.0, 2.0));
    EXPECT_FALSE(rtmc_is_greater(2.0, 2.0));
}

TEST(MathTests, IsGreaterEqual) {
    EXPECT_TRUE(rtmc_is_greater_equal(2.0, 1.0));
    EXPECT_FALSE(rtmc_is_greater_equal(1.0, 2.0));
    EXPECT_TRUE(rtmc_is_greater_equal(2.0, 2.0));
}

TEST(MathTests, IsLess) {
    EXPECT_FALSE(rtmc_is_less(2.0, 1.0));
    EXPECT_TRUE(rtmc_is_less(1.0, 2.0));
    EXPECT_FALSE(rtmc_is_less(2.0, 2.0));
}

TEST(MathTests, IsLessEqual) {
    EXPECT_FALSE(rtmc_is_less_equal(2.0, 1.0));
    EXPECT_TRUE(rtmc_is_less_equal(1.0, 2.0));
    EXPECT_TRUE(rtmc_is_less_equal(2.0, 2.0));
}




/*
    Vector Operation Tests
*/
TEST(MathTests, DotProduct) {
    int size = 5;
    std::vector<double> v1 = {1, 2, 3, 4, 5};
    std::vector<double> v2 = {5, 4, 3, 2, 1};

    EXPECT_EQ(35, rtmc_dot_product(v1.data(), v2.data(), size));
}

TEST(MathTests, CrossProduct_TypicalVectors) {
    int size = 3;
    std::vector<double> v1;
    std::vector<double> v2;
    std::vector<double> expected;
    double outputted[size];

    v1.assign({1, 0, 0});
    v2.assign({0, 1, 0});
    expected.assign({0, 0, 1});
    rtmc_cross_product(outputted, v1.data(), v2.data(), size);
    EXPECT_TRUE(rtmc_are_vectors_equal(expected.data(), outputted, size));

    v1.assign({150, 200, -100});
    v2.assign({0, 250, -250});
    expected.assign({-2.5e4, 3.75e4, 3.75e4});
    rtmc_cross_product(outputted, v1.data(), v2.data(), size);
    EXPECT_TRUE(rtmc_are_vectors_equal(expected.data(), outputted, size));
}

TEST(MathTests, CrossProduct_WrongSizeVectors) {
    int size = 5;
    std::vector<double> v1 = {1, 2, 3, 4, 5};
    std::vector<double> v2 = {1, 2, 3, 4, 5};
    double outputted[size];

    // Wrong size vectors should return [NAN, NAN, ..., NAN].
    // Since (NAN != NAN), we have to check the vector against itself knowing
    // that (X != X) is only true when X is NaN
    rtmc_cross_product(outputted, v1.data(), v2.data(), size);
    EXPECT_FALSE(rtmc_are_vectors_equal(outputted, outputted, size));
}

TEST(MathTests, ScalarMultiplication) {
    int size = 5;
    double s = -2.0;
    std::vector<double> v = {1, 2, 3, 4, 5};
    std::vector<double> expected = {-2, -4, -6, -8, -10};
    double outputted[size];

    rtmc_scalar_multiplication(outputted, v.data(), s, size);
    EXPECT_TRUE(rtmc_are_vectors_equal(expected.data(), outputted, size));
}

TEST(MathTests, ScalarDivision) {
    int size = 5;
    double s = -2.0;
    std::vector<double> v = {1, 2, 3, 4, 5};
    std::vector<double> expected = {-0.5, -1.0, -1.5, -2.0, -2.5};
    double outputted[size];

    rtmc_scalar_division(outputted, v.data(), s, size);
    EXPECT_TRUE(rtmc_are_vectors_equal(expected.data(), outputted, size));
}

TEST(MathTests, VectorAddition) {
    int size = 5;
    std::vector<double> v1 = {1, 2, 3, 4, 5};
    std::vector<double> v2 = {5, 4, 3, 2, 1};
    std::vector<double> expected = {6, 6, 6, 6, 6};
    double outputted[size];

    rtmc_vector_addition(outputted, v1.data(), v2.data(), size);
    EXPECT_TRUE(rtmc_are_vectors_equal(expected.data(), outputted, size));
}

TEST(MathTests, VectorSubtraction) {
    int size = 5;
    std::vector<double> v1 = {1, 2, 3, 4, 5};
    std::vector<double> v2 = {5, 4, 3, 2, 1};
    std::vector<double> expected = {-4, -2, 0, 2, 4};
    double outputted[size];

    rtmc_vector_subtraction(outputted, v1.data(), v2.data(), size);
    EXPECT_TRUE(rtmc_are_vectors_equal(expected.data(), outputted, size));
}

TEST(MathTests, VectorMagnitude) {
    int size = 5;
    std::vector<double> v = {1, 2, 3, 4, 5};

    EXPECT_EQ(sqrt(55), rtmc_vector_magnitude(v.data(), size));
}

TEST(MathTests, UnitVector) {
    int size = 5;
    std::vector<double> v = {1, 2, 3, 4, 5};
    std::vector<double> expected = {1/sqrt(55), 2/sqrt(55), 3/sqrt(55), 4/sqrt(55), 5/sqrt(55)};
    double outputted[size];

    rtmc_unit_vector(outputted, v.data(), size);
    EXPECT_TRUE(rtmc_are_vectors_equal(expected.data(), outputted, size));
}

TEST(MathTests, AreVectorsEqual) {
    int size = 5;
    std::vector<double> v1;
    std::vector<double> v2;

    v1.assign({1, 2, 3, 4, 5});
    v2.assign({1, 2, 3, 4, 5});
    EXPECT_TRUE(rtmc_are_vectors_equal(v1.data(), v2.data(), size));

    v1.assign({1, 2, 3, 4, 5});
    v2.assign({5, 4, 3, 2, 1});
    EXPECT_FALSE(rtmc_are_vectors_equal(v1.data(), v2.data(), size));
}

TEST(MathTests, IsDirectionEqual) {
    int size = 3;
    std::vector<double> v1;
    std::vector<double> v2;

    v1.assign({1, 2, 3});
    v2.assign({10, 20, 30});
    EXPECT_TRUE(rtmc_is_direction_equal(v1.data(), v2.data(), size));

    v1.assign({1, 2, 3});
    v2.assign({-1, -2, -3});
    EXPECT_FALSE(rtmc_is_direction_equal(v1.data(), v2.data(), size));
}



// test distance between two points
TEST(MathTests, Distance) {
    std::vector<double> p1;
    std::vector<double> p2;
    double distance;

    p1.assign({3, 10});
    p2.assign({-17, 100});
    distance = rtmc_distance(p1.data(), p2.data(), 2);
    EXPECT_TRUE(rtmc_is_equal(distance, sqrt(8.5e3)));

    p1.assign({1, 2, 3, 4, 5});
    p2.assign({5, 4, 3, 2, 1});
    distance = rtmc_distance(p1.data(), p2.data(), 5);
    EXPECT_TRUE(rtmc_is_equal(distance, 2*sqrt(10)));
}



// test sign function
TEST(MathTests, Sign) {
    EXPECT_EQ(rtmc_sign(5), 1);
    EXPECT_EQ(rtmc_sign(-5), -1);
    EXPECT_EQ(rtmc_sign(0), 0);
    EXPECT_EQ(rtmc_sign(INFINITY), 1);
    EXPECT_EQ(rtmc_sign(-INFINITY), -1);

    // check that sign(NAN) returns NAN
    double sign = rtmc_sign(NAN);
    EXPECT_NE(sign, sign);
}