#ifndef ADAPTIVEQUICKSELECT_HPP
#define ADAPTIVEQUICKSELECT_HPP

#include <vector>
#include <functional>
#include "Partition.hpp"
#include "MedianOfNinthers.hpp"
#include "../kdtree/KDNode.hpp"


class AdaptiveQuickselect {
private:
    typedef typename std::vector<KDNode*>::iterator iter_t;
    // 2^17 - Clang doesn't like pow() in constexprs
    static constexpr const long SAMPLING_THRESHOLD = 1 << 17;

//    std::vector<KDNode*>& array;
    unsigned long dimension = 0;

    Partition* partitioner;

public:
    //AdaptiveQuickselect() = default;

    AdaptiveQuickselect(unsigned long dimension_in):
//        array(array_in),
        dimension(dimension_in) 
	{
		this->partitioner = new Partition(dimension_in);
	}


    iter_t adaptiveQuickselect(iter_t begin_iter, iter_t end_iter, long nth_item) {
        iter_t pivot_iter;

        while (true) {
            if (nth_item == 0) {
                iter_t min_element = begin_iter;

                for (auto it = begin_iter + 1; it != end_iter; ++it) {
                    if (comp_lt(*it, *min_element, this->dimension)) min_element = it;
                }

                return this->partitioner->hoarePartition(begin_iter, end_iter, min_element - begin_iter);
            }

            if (std::distance(begin_iter, end_iter) <= 16) {
                pivot_iter = this->partitioner->hoarePartition(begin_iter, end_iter, nth_item);
            }

            else if (6 * nth_item <= std::distance(begin_iter, end_iter)) {
                pivot_iter = this->medianOfMinima(begin_iter, end_iter, nth_item);
            }

            else if (6 * nth_item >= 5 * std::distance(begin_iter, end_iter)) {
                pivot_iter = this->medianOfMaxima(begin_iter, end_iter, nth_item);
            }

            else {
                pivot_iter = medianOfNinthers(begin_iter, end_iter);
            }

            if (std::distance(begin_iter, pivot_iter) == nth_item) {
                return pivot_iter;
            }

            else if (std::distance(begin_iter, pivot_iter) > nth_item) {
                end_iter = pivot_iter + 1;
            }

            else {
                nth_item = nth_item - std::distance(begin_iter, pivot_iter) - 1;
                begin_iter = pivot_iter + 1;
            }
        }
    }

    void medianOfExtrema(
        const iter_t& subarray_begin,
        const iter_t& subarray_end,
        const iter_t& extrema_chunk_begin,
        const iter_t& extrema_chunk_end,
        const std::function<bool(const KDNode*, const KDNode*, const unsigned long&)> comp)
    {
        long extrema_to_compute = std::distance(extrema_chunk_begin, extrema_chunk_end);
        long chunk_size = std::distance(subarray_begin, subarray_end) / extrema_to_compute;

        for (long i = 0; i < extrema_to_compute; ++i) {
            iter_t chunk_extreme = subarray_begin + (i * chunk_size);

            for (iter_t traverser = chunk_extreme + 1; traverser != (subarray_begin + ((i + 1) * chunk_size)); ++traverser) {
                if (comp(*traverser, *chunk_extreme, this->dimension)) chunk_extreme = traverser;
            }

            if (comp(*chunk_extreme, *(extrema_chunk_begin + i), this->dimension)) {
                std::swap(*chunk_extreme, *(extrema_chunk_begin + i));
            }
        }
    }


    iter_t medianOfMinima(const iter_t& array_begin, const iter_t& array_end, const long& partition_index) {
        if (std::distance(array_begin, array_end) == 1) return array_begin;

        long minima_to_compute = partition_index * 2;

        iter_t minima_chunk_begin = array_begin;
        iter_t minima_chunk_end = array_begin + minima_to_compute;

        this->medianOfExtrema(minima_chunk_end, array_end, minima_chunk_begin, minima_chunk_end, comp_lt);

        this->adaptiveQuickselect(minima_chunk_begin, minima_chunk_end, partition_index);

        iter_t pivot = array_begin + partition_index;

        iter_t b = minima_chunk_end - 1;


        return this->partitioner->expandPartition(array_begin, array_begin, pivot, b, array_end);
    }


    iter_t medianOfMaxima(const iter_t& array_begin, const iter_t& array_end, const long& partition_index) {
        if (std::distance(array_begin, array_end) == 1) return array_begin;

        long array_length = std::distance(array_begin, array_end);
        long maxima_to_compute = (array_length - partition_index) * 2;

        iter_t maxima_chunk_begin = array_begin + (array_length - maxima_to_compute);
        iter_t maxima_chunk_end = array_end;

        iter_t subarray_begin = array_begin + (array_length % maxima_to_compute);

        this->medianOfExtrema(subarray_begin, maxima_chunk_begin, maxima_chunk_begin, maxima_chunk_end, comp_gt);

        this->adaptiveQuickselect(maxima_chunk_begin, maxima_chunk_end, array_length - partition_index);

        iter_t pivot = array_begin + partition_index;
        iter_t b = array_end - 1;

        return this->partitioner->expandPartition(array_begin, maxima_chunk_begin, pivot, b, array_end);
    }

    iter_t medianOfNinthers(const iter_t& array_begin, const iter_t& array_end) {
        long array_length = std::distance(array_begin, array_end);
        long sampling_constant = (array_length > SAMPLING_THRESHOLD) ? 64 : 1024;
        long subsample_size = (array_length / sampling_constant) / 3;

        if (subsample_size < 3) return this->partitioner->hoarePartition(array_begin, array_end, array_length / 2);

        long gap_size = (array_length - (3 * subsample_size)) / 4;

        iter_t left_gap_traverser = array_begin + gap_size;
        iter_t middle_gap_traverser = array_begin + (2 * gap_size) + subsample_size;
        iter_t middle_gap_begin = middle_gap_traverser;
        iter_t right_gap_traverser = array_begin + (3 * gap_size) + (2 * subsample_size);

        for (long i = 0; i < subsample_size / 3; ++i) {
            ninther(left_gap_traverser, middle_gap_traverser, right_gap_traverser, subsample_size);

            left_gap_traverser += 3;
            ++middle_gap_traverser;
            right_gap_traverser += 3;
        }

        this->adaptiveQuickselect(middle_gap_begin, middle_gap_begin + subsample_size, subsample_size / 2);

        iter_t pivot = middle_gap_begin + (subsample_size / 2);
        iter_t b = middle_gap_begin + subsample_size - 1;

        return this->partitioner->expandPartition(array_begin, middle_gap_begin, pivot, b, array_end);
    }
};

#endif
