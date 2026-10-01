#include "eigen_compare.h"
#include "eigen_compare.hpp"

namespace binary {

static std::size_t effective_length(
    const unsigned char* data,
    std::size_t len
) {
    while (len > 1 && data[len - 1] == 0) {
        --len;
    }

    return len;
}

int compare(
    const unsigned char* a,
    std::size_t a_len,
    const unsigned char* b,
    std::size_t b_len
) {
    a_len = effective_length(a, a_len);
    b_len = effective_length(b, b_len);

    if (a_len > b_len) {
        return 1;
    }

    if (a_len < b_len) {
        return -1;
    }

    Eigen::Map<const BinaryArray> va(
        a,
        static_cast<Eigen::Index>(a_len)
    );

    Eigen::Map<const BinaryArray> vb(
        b,
        static_cast<Eigen::Index>(b_len)
    );

    Eigen::Array<bool, Eigen::Dynamic, 1> different =
        (va != vb);
    if (!different.any()) {
        return 0;
    }

    for (Eigen::Index i = va.size() - 1; i >= 0; --i) {

        if (va(i) != vb(i)) {

            if (va(i) > vb(i)) {
                return 1;
            }

            return -1;
        }
    }

    return 0;
}

} // namespace binary



extern "C"
int eigen_compare_binary(
    const unsigned char* a,
    std::size_t a_len,
    const unsigned char* b,
    std::size_t b_len
) {
    return binary::compare(
        a,
        a_len,
        b,
        b_len
    );
}