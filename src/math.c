/*
    math.c
*/

#include <float.h>
#include <math.h>
#include <stdbool.h>
#include "rtmc_math.h"

/*
    Float comparison (akin to the "==" operator) is tricky because of floating
    point errors. To combat this, the simple approach is to use some sort of
    accuracy threshold. This works great for numbers that are close to zero,
    but not for larger numbers since floats lose precision as the magnitude
    increase.

    The second method is to use a threshold that scales with the float's
    magnitude. This works great for most numbers, but not so well for very
    small numbers (as numbers approach zero, so does the scaled threshold). 

    The best way is to set two thresholds: a scaled "typical_threshold", and
    a constant "SMALL_THRESHOLD". If the float falls within either of these,
    then the values being compared are close enough to be considered equal.

    To help make this clear, see the following Desmos plot:
    https://www.desmos.com/calculator/cckwruv1oy
*/
#define EPSILON 100 * DBL_EPSILON // about 2.22e-14
#define SMALL_THRESHOLD 100 * DBL_MIN // about 2.23e-306

bool rtmc_is_equal(double a, double b) {
    // trivial case (also handles +/- INFINITY)
    if(a == b)
        return true;

    double difference = fabs(a-b);
    double typical_threshold = EPSILON * (fabs(a) + fabs(b));

    return difference < fmax(SMALL_THRESHOLD, typical_threshold);
}

bool rtmc_is_greater(double a, double b) {
    return rtmc_is_equal(a, b) ? false : (a > b);
}

bool rtmc_is_greater_equal(double a, double b) {
    return rtmc_is_equal(a, b) ? true : (a > b); 
}

bool rtmc_is_less(double a, double b) {
    return rtmc_is_equal(a, b) ? false : (a < b);
}

bool rtmc_is_less_equal(double a, double b) {
    return rtmc_is_equal(a, b) ? true : (a < b);
}



/*
    Vector operations assume a 3 element double array as input
*/
double rtmc_dot_product(const double* v1, const double* v2, int size) {
    double sum = 0.0;
    for(int i = 0; i < size; i++) {
        sum += v1[i] * v2[i];
    }

    return sum; 
}

void rtmc_cross_product(double* cross_product, const double* v1, const double* v2, int size) {
    if(size == 3) {
        // return cross product
        cross_product[0] = v1[1] * v2[2] - v1[2] * v2[1];
        cross_product[1] = v1[2] * v2[0] - v1[0] * v2[2];
        cross_product[2] = v1[0] * v2[1] - v1[1] * v2[0];
    }
    else {
        // return [NAN, NAN, ..., NAN] vector
        for(int i = 0; i < size; i++) {
            cross_product[i] = NAN;
        }
    }
}

void rtmc_scalar_multiplication(double* scaled_vector, const double* v, double s, int size) {
    for(int i = 0; i < size; i++) {
        scaled_vector[i] = v[i] * s;
    }
}

void rtmc_scalar_division(double* scaled_vector, const double* v, double s, int size) {
    for(int i = 0; i < size; i++) {
        scaled_vector[i] = v[i] / s;
    }
}

void rtmc_vector_addition(double* vector_sum, const double* v1, const double* v2, int size) {
    for(int i = 0; i < size; i++) {
        vector_sum[i] = v1[i] + v2[i];
    }
}

void rtmc_vector_subtraction(double* vector_difference, const double* v1, const double* v2, int size) {
    for(int i = 0; i < size; i++) {
        vector_difference[i] = v1[i] - v2[i];
    }
}

double rtmc_vector_magnitude(const double* v, int size) {
    double sum_of_squares = 0.0;
    for(int i = 0; i < size; i++) {
        sum_of_squares += pow(v[i], 2);
    }

    return sqrt(sum_of_squares);
}

void rtmc_unit_vector(double* unit_vector, const double* v, int size) {
    double magnitude = rtmc_vector_magnitude(v, size);
    rtmc_scalar_division(unit_vector, v, magnitude, size);
}

bool rtmc_are_vectors_equal(const double* v1, const double* v2, int size) {
    for(int i = 0; i < size; i++) {
        if(!rtmc_is_equal(v1[i], v2[i])) {
            return false;
        }
    }

    return true;
}

bool rtmc_is_direction_equal(const double* v1, const double* v2, int size) {
    double unit_v1[size];
    double unit_v2[size];

    rtmc_unit_vector(unit_v1, v1, size);
    rtmc_unit_vector(unit_v2, v2, size);

    return rtmc_are_vectors_equal(unit_v1, unit_v2, size);
}



// get distance between two points
double rtmc_distance(const double* p1, const double* p2, int size) {
    double sum_of_squares = 0.0;
    for(int i = 0; i < size; i++) {
        // sum_of_squares += pow(v[i], 2);
        sum_of_squares += pow((p2[i] - p1[i]), 2);
    }

    return sqrt(sum_of_squares);
}


// return sign of number (and handle NaN)
double rtmc_sign(double x) {
    if(x != x)
        return NAN;
    else
        return (rtmc_is_greater_equal(x, 0)) ? 1 : -1;
}
