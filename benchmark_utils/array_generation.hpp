#include <cstdint>
#include <algorithm>


#pragma once


template <typename ItemT, int(*GeneratorT)()>
ItemT* generateArray(std::size_t size) {
    ItemT* a = new ItemT[size];
    std::generate(a, a + size, GeneratorT);

    return a;
}


template <typename ItemT, int(*GeneratorT)()>
ItemT** generateMatrix(std::size_t rows, std::size_t columns) {
    ItemT** a = new ItemT*[rows];

    for (std::size_t i = 0; i < rows; ++i) {
        a[i] = generateArray<ItemT, GeneratorT>(columns);
    }

    return a;
}