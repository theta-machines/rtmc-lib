/*
    rtmc_magic_numbers.h

    This file defines the constants (magic numbers) used throughout the
    library.
*/

#ifndef RTMC_MAGIC_NUMBERS_H
#define RTMC_MAGIC_NUMBERS_H

#ifdef __cplusplus
extern "C" {
#endif



/*
    Defines the number of axes (can be different from number of joints)
    This number can never exceed 12!!
*/
#define RTMC_NUM_AXES 12



/*
    Defines the number of path coefficients per axis
*/
#define RTMC_NUM_PATH_COEFFICIENTS 4 



/*
    Double types are accurate to 15 digits  (+1 sign, +1 decimal, +1 E,
    +1 E sign). Values with more than 19 characters will be truncated.
    
    For long numbers, use "1E-18" instead of "0.000000000000000001".
*/
#define RTMC_MAX_DECIMAL_LENGTH 19



/*
    Sets a path's feed rate to the maximum. (Possible because typical feed 
    rates must be positive)
*/
#define RTMC_RAPID_RATE -1



#ifdef __cplusplus
}
#endif

#endif // RTMC_MAGIC_NUMBERS_H
