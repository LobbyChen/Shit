#ifndef EIGEN_COMPARE_H
#define EIGEN_COMPARE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


int eigen_compare_binary(
    const unsigned char* a,
    size_t a_len,
    const unsigned char* b,
    size_t b_len
);

#ifdef __cplusplus
}
#endif

#endif