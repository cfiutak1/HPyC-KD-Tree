#ifndef MEDIANOFNINTHERS_HPP
#define MEDIANOFNINTHERS_HPP

#include <vector>
#include <cstdio>

#include "../kdtree/Point.hpp"


inline static std::vector<Point*>::iterator median3(std::vector<Point*>::iterator iter1, std::vector<Point*>::iterator iter2, std::vector<Point*>::iterator iter3) {
    if ((*iter1 <= *iter2 && *iter1 >= *iter3) || (*iter1 >= *iter2 && *iter1 <= *iter3)) return iter1;
    if ((*iter2 <= *iter1 && *iter2 >= *iter3) || (*iter2 >= *iter1 && *iter2 <= *iter3)) return iter2;
    return iter3;
}


inline std::vector<Point*>::iterator ninther(const std::vector<Point*>::iterator& array_l_begin, const std::vector<Point*>::iterator& array_m_begin, const std::vector<Point*>::iterator& array_r_begin, const long& subsample_size) {
    std::vector<Point*>::iterator nin = median3(
        median3(array_l_begin, array_m_begin, array_r_begin),
        median3(array_l_begin + 1, array_m_begin + (subsample_size / 3), array_r_begin + 1),
        median3(array_l_begin + 2, array_m_begin + (2 * (subsample_size / 3)), array_r_begin + 2)
    );

    std::swap(*nin, *(array_m_begin + (subsample_size / 3)));

    return nin;
}

#endif
