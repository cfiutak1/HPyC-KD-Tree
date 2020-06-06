#ifndef COMPARATORS_HPP
#define COMPARATORS_HPP

inline bool comp_lt(const float* n1, const float* n2, const unsigned long& dimension) {
    return n1[dimension] < n2[dimension];
}

inline bool comp_gt(const float* n1, const float* n2, const unsigned long& dimension) {
    return n1[dimension] > n2[dimension];
}

inline bool comp_lte(const float* n1, const float* n2, const unsigned long& dimension) {
    return n1[dimension] <= n2[dimension];
}

inline bool comp_gte(const float* n1, const float* n2, const unsigned long& dimension) {
    return n1[dimension] >= n2[dimension];
}

#endif