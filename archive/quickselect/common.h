/*          Copyright Andrei Alexandrescu, 2016-.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://boost.org/LICENSE_1_0.txt)
 */

#pragma once
#include <algorithm>


/**
Implements Hoare partition.
*/
template <class T>
T* pivotPartition(T* r, const unsigned int& depth, size_t k, size_t length) {
    std::swap(*r, r[k]);
    size_t lo = 1, hi = length - 1;

    for (;; ++lo, --hi) {
        for (;; ++lo) {
            if (lo > hi) { goto loop_done; }
            if (r[lo][depth] >= r[0][depth]) { break; }
        }
        // found the left bound:  r[lo] >= r[0]
        for (; r[0][depth] < r[hi][depth]; --hi);

        if (lo >= hi) { break; }

        // found the right bound: r[hi][depth] <= r[0][depth], swap & make progress
        std::swap(r[lo], r[hi]);
    }

loop_done:
    --lo;
    std::swap(r[lo], *r);
    return r + lo;
}



/**
Returns the index of the median of r[a], r[b], and r[c] without writing
anything.
*/
template <class T>
size_t medianIndex(const T* r, const unsigned int& depth, size_t a, size_t b, size_t c) {
    if (r[a][depth] > r[c][depth]) { std::swap(a, c); }
    if (r[b][depth] > r[c][depth]) { return c; }
    if (r[b][depth] < r[a][depth]) { return a; }
    return b;
}

/**
Returns the index of the median of r[a], r[b], r[c], and r[d] without writing
anything. If leanRight is true, computes the upper median. Otherwise, conputes
the lower median.
*/
template <bool leanRight, class T>
static size_t medianIndex(T* r, const unsigned int& depth, size_t a, size_t b, size_t c, size_t d) {
    if (r[d][depth] < r[c][depth]) { std::swap(c, d); }

    /* static */
    if (leanRight) {
        if (r[c][depth] < r[a][depth]) {
            return medianIndex(r, depth, a, b, d);
        }
    }
    else {
        if (r[d][depth] >= r[a][depth]) {
            return medianIndex(r, depth, a, b, c);
        }
    }

    // Could return medianIndex(r, b, c, d) but we already know r[c][depth] <= r[d][depth]
    if (r[b][depth] <= r[c][depth]) return c;
    if (r[b][depth] > r[d][depth]) return d;
    return b;
}

/**
Tukey's Ninther: compute the median of r[_1], r[_2], r[_3], then the median of
r[_4], r[_5], r[_6], then the median of r[_7], r[_8], r[_9], and then swap the
median of those three medians into r[_5].
*/
template <class T>
void ninther(T* r, const unsigned int& depth, size_t _1, size_t _2, size_t _3, size_t _4, size_t _5,
    size_t _6, size_t _7, size_t _8, size_t _9)
{
    _2 = medianIndex(r, depth, _1, _2, _3);
    _8 = medianIndex(r, depth, _7, _8, _9);
    if (r[_2][depth] > r[_8][depth]) std::swap(_2, _8);
    if (r[_4][depth] > r[_6][depth]) std::swap(_4, _6);
    // Here we know that r[_2] and r[_8] are the other two medians and that
    // r[_2] <= r[_8]. We also know that r[_4] <= r[_6]
    if (r[_5][depth] < r[_4][depth])
    {
        // r[_4] is the median of r[_4], r[_5], r[_6]
    }
    else if (r[_5][depth] > r[_6][depth])
    {
        // r[_6] is the median of r[_4], r[_5], r[_6]
        _4 = _6;
    }
    else
    {
        // Here we know r[_5] is the median of r[_4], r[_5], r[_6]
        if (r[_5][depth] < r[_2][depth]) return std::swap(r[_5], r[_2]);
        if (r[_5][depth] > r[_8][depth]) return std::swap(r[_5], r[_8]);
        // This is the only path that returns with no swap
        return;
    }
    // Here we know r[_4] is the median of r[_4], r[_5], r[_6]
    if (r[_4][depth] < r[_2][depth]) _4 = _2;
    else if (r[_4][depth] > r[_8][depth]) _4 = _8;
    std::swap(r[_5], r[_4]);
}

/**
Input assumptions:

(a) hi <= rite
(c) the range r[0 .. hi] contains elements no smaller than r[0]

Output guarantee: same as Hoare partition using r[0] as pivot. Returns the new
position of the pivot.
*/
template <class T>
size_t expandPartitionRight(T* r, const unsigned int& depth, size_t hi, size_t rite) {
    size_t pivot = 0;

    // First loop: spend r[pivot .. hi]
    for (; pivot < hi; --rite)
    {
        if (rite == hi) goto done;
        if (r[rite][depth] >= r[0][depth]) continue;
        ++pivot;
        std::swap(r[rite], r[pivot]);
    }
    // Second loop: make left and pivot meet
    for (; rite > pivot; --rite)
    {
        if (r[rite][depth] >= r[0][depth]) continue;
        while (rite > pivot)
        {
            ++pivot;
            if (r[0][depth] < r[pivot][depth])
            {
                std::swap(r[rite], r[pivot]);
                break;
            }
        }
    }

done:
    std::swap(r[0], r[pivot]);
    return pivot;
}

/**
Input assumptions:

(a) lo > 0, lo <= pivot
(b) the range r[lo .. pivot] already contains elements no greater than r[pivot]

Output guarantee: Same as Hoare partition around r[pivot]. Returns the new
position of the pivot.

*/
template <class T>
size_t expandPartitionLeft(T* r, const unsigned int& depth, size_t lo, size_t pivot) {
    size_t left = 0;
    const auto oldPivot = pivot;

    for (; lo < pivot; ++left) {
        if (left == lo) { goto done; }

        if (r[oldPivot][depth] >= r[left][depth]) { continue; }

        --pivot;

        std::swap(r[left], r[pivot]);
    }

    // Second loop: make left and pivot meet
    for (;; ++left) {
        if (left == pivot) { break; }

        if (r[oldPivot][depth] >= r[left][depth]) { continue; }

        for (;;) {
            if (left == pivot) { goto done; }
            --pivot;

            if (r[pivot][depth] < r[oldPivot][depth]) {
                std::swap(r[left], r[pivot]);
                break;
            }
        }
    }

done:
    std::swap(r[oldPivot], r[pivot]);
    return pivot;
}

/**
Input assumptions:

(a) lo <= pivot, pivot < hi, hi <= length
(b) the range r[lo .. pivot] already contains elements no greater than
r[pivot]
(c) the range r[pivot .. hi] already contains elements no smaller than
r[pivot]

Output guarantee: Same as Hoare partition around r[pivot], returning the new
position of the pivot.
*/
template <class T>
size_t expandPartition(T* r, const unsigned int& depth, size_t lo, size_t pivot, size_t hi, size_t length) {
    --hi;
    --length;
    size_t left = 0;

    for (;; ++left, --length) {
        for (;; ++left) {
            if (left == lo) {
                return pivot + expandPartitionRight(r + pivot, depth, hi - pivot, length - pivot);
            }

            if (r[left][depth] > r[pivot][depth]) { break; }
        }
        for (;; --length) {
            if (length == hi) {
                return left + expandPartitionLeft(r + left, depth, lo - left, pivot - left);
            }
            if (r[pivot][depth] >= r[length][depth]) { break; }
        }
        std::swap(r[left], r[length]);
    }
}
