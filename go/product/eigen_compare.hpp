#ifndef EIGEN_COMPARE_HPP
#define EIGEN_COMPARE_HPP

#include <cstddef>
#include <Eigen/Core>

namespace binary {

using BinaryArray =
    Eigen::Array<unsigned char, Eigen::Dynamic, 1>;

int compare(
    const unsigned char* a,
    std::size_t a_len,
    const unsigned char* b,
    std::size_t b_len
);

}

#endif