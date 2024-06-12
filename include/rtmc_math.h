/*
    rtmc_math.h
*/

#ifndef RTMC_MATH_H
#define RTMC_MATH_H

#ifdef __cplusplus
extern "C" {
#endif



// common constants
#define RTMC_PI 3.141592653589793238
#define RTMC_E 2.718281828459045235

// double comparisons
bool rtmc_is_equal(double a, double b);
bool rtmc_is_greater(double a, double b);
bool rtmc_is_greater_equal(double a, double b);
bool rtmc_is_less(double a, double b);
bool rtmc_is_less_equal(double a, double b);

// vector operations (a vector is an array doubles)
// note that rtmc_cross_product returns [NAN, NAN, ..., NAN] if size != 3
double rtmc_dot_product(const double* v1, const double* v2, int size);
void rtmc_cross_product(double* cross_product, const double* v1, const double* v2, int size);
void rtmc_scalar_multiplication(double* scaled_vector, const double* v, double s, int size);
void rtmc_scalar_division(double* scaled_vector, const double* v, double s, int size);
void rtmc_vector_addition(double* vector_sum, const double* v1, const double* v2, int size);
void rtmc_vector_subtraction(double* vector_difference, const double* v1, const double* v2, int size);
double rtmc_vector_magnitude(const double* v, int size);
void rtmc_unit_vector(double* unit_vector, const double* v, int size);
bool rtmc_are_vectors_equal(const double* v1, const double* v2, int size);
bool rtmc_is_direction_equal(const double* v1, const double* v2, int size);

// get distance between two points
double rtmc_distance(const double* p1, const double* p2, int size);



#ifdef __cplusplus
}
#endif

#endif // RTMC_MATH_H
