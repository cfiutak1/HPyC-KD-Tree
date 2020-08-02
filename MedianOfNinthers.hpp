#ifndef MEDIANOFNINTHERS_HPP
#define MEDIANOFNINTHERS_HPP

#include <vector>

#include "Comparators.hpp"


inline static std::vector<float*>::iterator median3(
    std::vector<float*>::iterator iter1,
    std::vector<float*>::iterator iter2,
    std::vector<float*>::iterator iter3,
    const unsigned long& dimension)
{
    // TODO Maybe replace with NXOR?
    if ((comp_lte(*iter1, *iter2, dimension) && comp_gte(*iter1, *iter3, dimension)) || (comp_gte(*iter1, *iter2, dimension) && comp_lte(*iter1, *iter3, dimension))) { return iter1; }

    if ((comp_lte(*iter2, *iter1, dimension) && comp_gte(*iter2, *iter3, dimension)) || (comp_gte(*iter2, *iter1, dimension) && comp_lte(*iter2, *iter3, dimension))) { return iter2; }

    return iter3;
}


inline std::vector<float*>::iterator ninther(
    const std::vector<float*>::iterator& array_l_begin,
    const std::vector<float*>::iterator& array_m_begin,
    const std::vector<float*>::iterator& array_r_begin,
    const long& subsample_size,
    const unsigned long& dimension)
{
    std::vector<float*>::iterator nin = median3(
        median3(array_l_begin, array_m_begin, array_r_begin, dimension),
        median3(array_l_begin + 1, array_m_begin + (subsample_size / 3), array_r_begin + 1, dimension),
        median3(array_l_begin + 2, array_m_begin + (2 * (subsample_size / 3)), array_r_begin + 2, dimension),
        dimension
    );

    std::swap(*nin, *(array_m_begin + (subsample_size / 3)));

    return nin;
}

#endif
