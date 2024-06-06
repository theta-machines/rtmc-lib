/*
    rtmc_parser.h
*/

#ifndef RTMC_PARSER_H
#define RTMC_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif



#include "rtmc_path.h"

/*
    Parse a g-code string (called a block) and directly modify the `path` 
    argument. G-code blocks must be terminated with '\r', '\n', or '\0'.

    Arguments:
    block - a line of g-code
    start_coords - coordinates before the g-code block is executed
    path - the output path data (parsed g-code)

    A note on decimal values:
    Decimal values with more than RTMC_MAX_DECIMAL_LENGTH characters will be
    truncated. For long numbers, use "1E-18" instead of "0.000000000000000001".
*/
void rtmc_parse(const char* block, const double* start_coords, rtmc_path_t* path);



#ifdef __cplusplus
}
#endif

#endif // RTMC_PARSER_H
