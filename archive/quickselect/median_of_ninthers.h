/*          Copyright Andrei Alexandrescu, 2016-.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://boost.org/LICENSE_1_0.txt)
 */

#pragma once
#include "common.h"
#include <algorithm>


template <class T>
void adaptiveQuickselect(T* beg, const unsigned int& depth, size_t n, size_t length);

/**
Median of minima
*/
template <class T>
size_t medianOfMinima(T* const r, const unsigned int& depth, const size_t n, const size_t length) {
    const size_t subset = n * 2, computeMinOver = (length - subset) / subset;

    for (size_t i = 0, j = subset; i < subset; ++i) {
        const auto limit = j + computeMinOver;
        size_t minIndex = j;

        while (++j < limit) {
            if (r[j][depth] < r[minIndex][depth]) {
                minIndex = j;
            }
        }

        if (r[minIndex][depth] < r[i][depth]) {
            std::swap(r[i], r[minIndex]);
        }
    }
    adaptiveQuickselect(r, depth, n, subset);

    return expandPartition(r, depth, 0, n, subset, length);
}


/**
Median of maxima
*/
template <class T>
size_t medianOfMaxima(T* const r, const unsigned int& depth, const size_t n, const size_t length) {
    const size_t subset = (length - n) * 2,
        subsetStart = length - subset,
        computeMaxOver = subsetStart / subset;

    for (size_t i = subsetStart, j = i - subset * computeMaxOver; i < length; ++i) {
        const auto limit = j + computeMaxOver;
        size_t maxIndex = j;

        while (++j < limit) {
            if (r[j][depth] > r[maxIndex][depth]) {
                maxIndex = j;
            }
        }

        if (r[maxIndex][depth] > r[i][depth]) {
            std::swap(r[i], r[maxIndex]);
        }
    }
    adaptiveQuickselect(r + subsetStart, depth, length - n, subset);
    return expandPartition(r, depth, subsetStart, n, length, length);
}


/**
Partitions r[0 .. length] using a pivot of its own choosing. Attempts to pick a
pivot that approximates the median. Returns the position of the pivot.
*/
template <class T>
size_t medianOfNinthers(T* const r, const unsigned int& depth, const size_t length) {
    const auto frac =
        length <= 1024 ? length / 12 :
        length <= 128 * 1024 ? length / 64
        : length / 1024;
    auto pivot = frac / 2;
    const auto lo = length / 2 - pivot, hi = lo + frac;

    const auto gap = (length - 9 * frac) / 4;
    auto a = lo - 4 * frac - gap, b = hi + gap;
    for (size_t i = lo; i < hi; ++i, a += 3, b += 3) {
        ninther(r, depth, a, i - frac, b, a + 1, i, b + 1, a + 2, i + frac, b + 2);
    }

    adaptiveQuickselect(r + lo, depth, pivot, frac);
    return expandPartition(r, depth, lo, lo + pivot, hi, length);
}


/**
Quickselect driver for medianOfNinthers, medianOfMinima, and medianOfMaxima.
Dispathes to each depending on the relationship between n (the sought order
statistics) and length.
*/
template <class T>
void adaptiveQuickselect(T* r, const unsigned int& depth, size_t n, size_t length) {
    while (true) {
        // Decide strategy for partitioning
        if (n == 0) {
            // That would be the max
            auto pivot = n;

            for (++n; n < length; ++n) {
                if (r[n][depth] < r[pivot][depth]) {
                    pivot = n;
                }
            }

            std::swap(r[0], r[pivot]);

            return;
        }

        if (n + 1 == length) {
            // That would be the min
            auto pivot = 0;
            for (n = 1; n < length; ++n) {
                if (r[pivot][depth] < r[n][depth]) {
                    pivot = n;
                }
            }
            std::swap(r[pivot], r[length - 1]);

            return;
        }

        size_t pivot;

        if (length <= 16) { pivot = pivotPartition(r, depth, n, length) - r; }
        else if (n * 6 <= length) { pivot = medianOfMinima(r, depth, n, length); }
        else if (n * 6 >= length * 5) { pivot = medianOfMaxima(r, depth, n, length); }
        else { pivot = medianOfNinthers(r, depth, length); }

        // See how the pivot fares
        if (pivot == n) { return; }
        if (pivot > n) { length = pivot; }
        else {
            ++pivot;
            r += pivot;
            length -= pivot;
            n -= pivot;
        }
    }
}
