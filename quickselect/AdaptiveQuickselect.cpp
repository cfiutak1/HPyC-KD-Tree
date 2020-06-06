#include "AdaptiveQuickselect.hpp"

#include <vector>


std::vector<float*>::iterator AdaptiveQuickselect::adaptiveQuickselect(std::vector<float*>::iterator begin_iter, std::vector<float*>::iterator end_iter, long nth_item) {
    std::vector<float*>::iterator pivot_iter;

    while (true) {
        if (nth_item == 0) {
            std::vector<float*>::iterator min_element = begin_iter;

            for (auto it = begin_iter + 1; it != end_iter; ++it) {
                if (comp_lt(*it, *min_element, this->dimension)) min_element = it;
            }

            return this->partitioner.hoarePartition(begin_iter, end_iter, min_element - begin_iter);
        }

        if (std::distance(begin_iter, end_iter) <= 16) {
            pivot_iter = this->partitioner.hoarePartition(begin_iter, end_iter, nth_item);
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


std::vector<float*>::iterator AdaptiveQuickselect::medianOfMinima(const std::vector<float*>::iterator& array_begin, const std::vector<float*>::iterator& array_end, const long& partition_index) {
    if (std::distance(array_begin, array_end) == 1) return array_begin;

    long minima_to_compute = partition_index * 2;

    std::vector<float*>::iterator minima_chunk_begin = array_begin;
    std::vector<float*>::iterator minima_chunk_end = array_begin + minima_to_compute;

    this->medianOfExtrema<comp_lt>(minima_chunk_end, array_end, minima_chunk_begin, minima_chunk_end);

    this->adaptiveQuickselect(minima_chunk_begin, minima_chunk_end, partition_index);

    std::vector<float*>::iterator pivot = array_begin + partition_index;

    std::vector<float*>::iterator b = minima_chunk_end - 1;


    return this->partitioner.expandPartition(array_begin, array_begin, pivot, b, array_end);
}


std::vector<float*>::iterator AdaptiveQuickselect::medianOfMaxima(const std::vector<float*>::iterator& array_begin, const std::vector<float*>::iterator& array_end, const long& partition_index) {
    if (std::distance(array_begin, array_end) == 1) return array_begin;

    long array_length = std::distance(array_begin, array_end);
    long maxima_to_compute = (array_length - partition_index) * 2;

    std::vector<float*>::iterator maxima_chunk_begin = array_begin + (array_length - maxima_to_compute);
    std::vector<float*>::iterator maxima_chunk_end = array_end;

    std::vector<float*>::iterator subarray_begin = array_begin + (array_length % maxima_to_compute);

    this->medianOfExtrema<comp_gt>(subarray_begin, maxima_chunk_begin, maxima_chunk_begin, maxima_chunk_end);

    this->adaptiveQuickselect(maxima_chunk_begin, maxima_chunk_end, array_length - partition_index);

    std::vector<float*>::iterator pivot = array_begin + partition_index;
    std::vector<float*>::iterator b = array_end - 1;

    return this->partitioner.expandPartition(array_begin, maxima_chunk_begin, pivot, b, array_end);
}



std::vector<float*>::iterator AdaptiveQuickselect::medianOfNinthers(const std::vector<float*>::iterator& array_begin, const std::vector<float*>::iterator& array_end) {
    long array_length = std::distance(array_begin, array_end);
    long sampling_constant = (array_length > SAMPLING_THRESHOLD) ? 64 : 1024;
    long subsample_size = (array_length / sampling_constant) / 3;

    if (subsample_size < 3) return this->partitioner.hoarePartition(array_begin, array_end, array_length / 2);

    long gap_size = (array_length - (3 * subsample_size)) / 4;

    std::vector<float*>::iterator left_gap_traverser = array_begin + gap_size;
    std::vector<float*>::iterator middle_gap_traverser = array_begin + (2 * gap_size) + subsample_size;
    std::vector<float*>::iterator middle_gap_begin = middle_gap_traverser;
    std::vector<float*>::iterator right_gap_traverser = array_begin + (3 * gap_size) + (2 * subsample_size);

    for (long i = 0; i < subsample_size / 3; ++i) {
        this->ninther(left_gap_traverser, middle_gap_traverser, right_gap_traverser, subsample_size);

        left_gap_traverser += 3;
        ++middle_gap_traverser;
        right_gap_traverser += 3;
    }

    this->adaptiveQuickselect(middle_gap_begin, middle_gap_begin + subsample_size, subsample_size / 2);

    std::vector<float*>::iterator pivot = middle_gap_begin + (subsample_size / 2);
    std::vector<float*>::iterator b = middle_gap_begin + subsample_size - 1;

    return this->partitioner.expandPartition(array_begin, middle_gap_begin, pivot, b, array_end);
}
