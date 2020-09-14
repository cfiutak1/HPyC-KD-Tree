#include <cmath>
#include <algorithm>
#include <cstdio>
#include <cstdint>

#pragma once


template <std::size_t BlockSize=2048, std::size_t CacheLineSize=64>
class AdaptiveCacheAwareBlockquickselect {
private:
    float** nodes;
    uint64_t num_dimensions;
    unsigned int depth;
    static constexpr std::size_t ItemsPerCacheLine = CacheLineSize / sizeof(float);

public:
    AdaptiveCacheAwareBlockquickselect() = delete;

    AdaptiveCacheAwareBlockquickselect(float** nodes_in, uint64_t num_dimensions_in, unsigned int depth_in):
        nodes(nodes_in),
        num_dimensions(num_dimensions_in),
        depth(depth_in)
    {}

//    inline float* nodeAt(const uint64_t node_index) const {
////        printf("%s:%d fetching node at pos %lu\n", __FILE__, __LINE__, (node_index * this->num_dimensions));
//        return this->nodes + (node_index * this->num_dimensions);
//    }

    inline float& cellAt(const std::size_t index) const {
        return this->nodes[this->depth][index];
    }

//    inline float& nodeAtPos(const std::size_t index) const {
//        return this->nodes[this->depth][index];
//    }

    inline void swap(const std::size_t index1, const std::size_t index2) {
        for (auto i = 0; i < this->num_dimensions; ++i) {
            std::swap(
                this->nodes[i][index1],
                this->nodes[i][index2]
            );
        }
    }

    inline void compareAndSwap(const std::size_t index1, const std::size_t index2) {
        if (this->cellAt(index2) < this->cellAt(index1)) {
            this->swap(index1, index2);
        }
    }

    /*
     * Sorting network for an input of size 3. Sorts the first, middle, and final items of the array, then places the
     * median of the three values at begin + 1.
     *
     * Returns the median of the 3 items sampled, which will serve as the pivot iterator.
     */
    inline std::size_t medianOf3(const std::size_t begin, const std::size_t end) {
        this->compareAndSwap(begin, begin + ((end - begin) / 2));
        this->compareAndSwap(begin + ((end - begin) / 2), end - 1);
        this->compareAndSwap(begin, begin + ((end - begin) / 2));

        this->swap(begin + 1, begin + ((end - begin) / 2));

        return begin + 1;
    }

    /*
     * Somewhat naive implementation of block partition that handles two cases:
     *      (1) When staticBlockPartition() is finished with its main loop and has remaining elements to partition.
     *      (2) Arrays of size less than (2 * BlockSize) + 1, which are too small for staticBlockPartition().
     *
     * The function has two main phases - the partitioning phase and the cleanup phase.
     *
     * The partitioning phase consists of the following steps:
     *      (1) Divide the array chunk into two slices - a left and right slice.
     *      (2) Iterate over both slices in the same loop and fill the buffers.
     *          (left slice) Add the index (relative to unpartitioned_range_begin) of any item !comp() to the pivot to the left buffer.
     *          (right slice) Add the index (relative to last_unpartitioned_item) of any item comp() to the pivot to the right buffer.
     *      (3) Find the number of mutual swaps between the buffers, which is the size of the smaller buffer.
     *      (4) Iterate through the two buffers, swapping out of place items until the number of mutual swaps has been reached.
     *
     * The cleanup phase consists of the following steps:
     *      (1) unpartitioned_range_begin begins tracking the beginning of the !comp() partition of the array chunk.
     *      (2) If either buffer has remaining elements to swap, the elements are instead swapped to the extrema closest to the center
     *          of the respective partition.
     *      (3) unpartitioned_range_begin is updated to reflect the new beginning of the !comp() partition.
     *      (4) The pivot is moved to unpartitioned_range_begin - 1 and is used as the return value.
     *
     * Assumes that:
     *      (1) pivot < unpartitioned_range_begin
     *      (2) unpartitioned_range_begin <= last_unpartitioned_item TODO - verify that this method works on arrays of size 0 lol
     *      (3) left_buffer and right_buffer have space for at least (remaining_unpartitioned_size / 2) + 1 items
     *
     * Returns a fully partitioned array around pivot.
     */
    // RIP 640x480 users. Reading this signature in its entirety may take a few years.
    inline std::size_t dynamicBlockPartition(std::size_t pivot, std::size_t unpartitioned_range_begin, std::size_t last_unpartitioned_item, std::size_t* left_buffer, std::size_t* right_buffer) {
        /*
         * Partitioning phase
         */
        // + 1 because last_unpartitioned_item is inclusive
        std::size_t remaining_unpartitioned_size = last_unpartitioned_item - unpartitioned_range_begin + 1;

        std::size_t left_buffer_size = 0;
        std::size_t right_buffer_size = 0;

        // Iterate over the array chunk, populating the left and right swap buffers.
        for (size_t i = 0; i < remaining_unpartitioned_size / 2; ++i) {
            left_buffer[left_buffer_size] = i;
            left_buffer_size += (this->cellAt(pivot) < this->cellAt(unpartitioned_range_begin + i));

            right_buffer[right_buffer_size] = i;
            right_buffer_size += (this->cellAt(last_unpartitioned_item - i) < this->cellAt(pivot));
        }

        // The number of items to swap is the size of the smaller bucket.
        std::size_t num_mutual_swaps = std::min(left_buffer_size, right_buffer_size);

        // Perform a mutual exchange of misplaced values.
        for (size_t i = 0; i < num_mutual_swaps; ++i) {
            this->swap(
                unpartitioned_range_begin + left_buffer[i],
                last_unpartitioned_item - right_buffer[i]
            );
        }

        /*
         * Cleanup phase
         */
        std::size_t initial_unpartitioned_range_begin = unpartitioned_range_begin;

        // Update unpartitioned_range_begin to be at the beginning of the !comp() partition. If the array chunk is odd, the middle
        // item has not been inspected. If this item compares with the pivot, the comp() partition's size is incremented by 1.
        unpartitioned_range_begin += (remaining_unpartitioned_size / 2);
        unpartitioned_range_begin += ((remaining_unpartitioned_size % 2) && (this->cellAt(unpartitioned_range_begin) < this->cellAt(pivot)));

        // If there are remaining elements in the left buffer, move them to the right end of the comp() partition.
        // The elements closer to the chunk center are moved first, then the elements at the edges of the array.
        for (size_t i = 0; i < left_buffer_size - num_mutual_swaps; ++i) {
            this->swap(
                initial_unpartitioned_range_begin + left_buffer[left_buffer_size - 1 - i],
                unpartitioned_range_begin - 1 - i
           );
        }

        // Ditto for the left buffer and the !comp() partition.
        for (size_t i = 0; i < right_buffer_size - num_mutual_swaps; ++i) {
            this->swap(
                last_unpartitioned_item - right_buffer[right_buffer_size - 1 - i],
                unpartitioned_range_begin + i
            );
        }

        left_buffer_size -= num_mutual_swaps;
        right_buffer_size -= num_mutual_swaps;

        // Move unpartitioned_range_begin to the left/right, depending on which (if any) of the buffers had remaining items.
        unpartitioned_range_begin -= left_buffer_size;
        unpartitioned_range_begin += right_buffer_size;

        // Swap the pivot with the last item in the comp() partition.
        this->swap(unpartitioned_range_begin - 1, pivot);

        return unpartitioned_range_begin - 1;
    }

    /*
     * Delegator function for the above definition of dynamicBlockPartition(). Dynamically allocates buffers based on the
     * chunk size. Called by the partition delegator when the size of the array to partition is too small for
     * staticBlockPartition().
     *
     * Returns the result of the main dynamicBlockPartition() function.
     */
    inline std::size_t dynamicBlockPartition(std::size_t pivot, std::size_t unpartitioned_range_begin, std::size_t last_unpartitioned_item) {
        alignas(64) std::size_t* left_buffer = new std::size_t[((last_unpartitioned_item - unpartitioned_range_begin + 1) >> 1u) + 1];
        alignas(64) std::size_t* right_buffer = new std::size_t[((last_unpartitioned_item - unpartitioned_range_begin + 1) >> 1u) + 1];

        pivot = dynamicBlockPartition(pivot, unpartitioned_range_begin, last_unpartitioned_item, left_buffer, right_buffer);

        delete[] left_buffer;
        delete[] right_buffer;

        return pivot;
    }


    /*
     *
     * Works very similarly to dynamicBlockPartition(), except it uses statically allocated buffers of size BlockSize.
     *
     * Steps:
     *      (1) Processes two blocks, one on each side of the array chunk, until the remaining unpartitioned range < (2 * BlockSize) + 1.
     *      (2) Then, performs cleanup similarly to dynamicBlockPartition().
     *      (3) Sends the remaining unpartitioned array chunk to dynamicBlockPartition().
     *
     * Returns the correctly positioned pivot from dynamicBlockPartition().
     */
    inline std::size_t staticBlockPartition(std::size_t pivot, std::size_t unpartitioned_range_begin, std::size_t unpartitioned_range_end) {
        constexpr std::size_t BLOCK_PARTITION_THRESHOLD = BlockSize * 2;

        std::size_t last_item = unpartitioned_range_end - 1;

        alignas(64) std::size_t left_buffer[BlockSize];
        alignas(64) std::size_t right_buffer[BlockSize];
        std::size_t left_buffer_size = 0;
        std::size_t right_buffer_size = 0;

        std::size_t num_mutual_swaps = 0;

        /*
         * Partitioning phase
         */
        while (static_cast<std::size_t>(last_item - unpartitioned_range_begin + 1) > BLOCK_PARTITION_THRESHOLD) {
            // If both buffers are empty, process the next left and right blocks, recording any indices which are not
            // correctly partitioned.
            if (left_buffer_size == 0 && right_buffer_size == 0) {
                for (std::size_t i = 0; i < BlockSize; ) {
                    for (std::size_t j = 0; j < ItemsPerCacheLine; ++j) {
                        left_buffer[left_buffer_size] = i;
                        right_buffer[right_buffer_size] = i;

                        left_buffer_size += this->cellAt(pivot) < this->cellAt(unpartitioned_range_begin + i);
                        right_buffer_size += this->cellAt(last_item - i) < this->cellAt(pivot);
                        ++i;
                    }
                }
            }

                // If only the left bucket is empty, fill it.
            else if (left_buffer_size == 0) {
                for (std::size_t i = 0; i < BlockSize; ) {
                    for (std::size_t j = 0; j < ItemsPerCacheLine; ++j) {
                        left_buffer[left_buffer_size] = i;
                        left_buffer_size += this->cellAt(pivot) < this->cellAt(unpartitioned_range_begin + i);
                        ++i;
                    }
                }
            }

                // If only the right bucket is empty, fill it.
            else if (right_buffer_size == 0) {
                for (std::size_t i = 0; i < BlockSize; ) {
                    for (std::size_t j = 0; j < ItemsPerCacheLine; ++j) {
                        right_buffer[right_buffer_size] = i;
                        right_buffer_size += this->cellAt(last_item - i) < this->cellAt(pivot);
                        ++i;
                    }
                }
            }

            // The number of items to swap is the size of the smaller bucket
            num_mutual_swaps = std::min(left_buffer_size, right_buffer_size);

            // Perform a mutual exchange of misplaced values.
            for (std::size_t i = 0; i < num_mutual_swaps; ++i) {
                this->swap(
                    unpartitioned_range_begin + left_buffer[left_buffer_size - 1 - i],
                    last_item - right_buffer[right_buffer_size - 1 - i]
                );
            }

            left_buffer_size -= num_mutual_swaps;
            right_buffer_size -= num_mutual_swaps;

            // If left_buffer_size == 0, add BlockSize to unpartitioned_range_begin
            unpartitioned_range_begin += (0 - (left_buffer_size == 0)) & BlockSize;
            // If right_buffer_size == 0, subtract BlockSize from last_item
            last_item -= (0 - (right_buffer_size == 0)) & BlockSize;
        }

        /*
         * Cleanup phase for final iteration of main loop
         */
        // If left_buffer_size != 0, add BlockSize to begin
        unpartitioned_range_begin += (0 - (left_buffer_size != 0)) & BlockSize;

        // If right_buffer_size != 0, subtract BlockSize from last
        last_item -= (0 - (right_buffer_size != 0)) & BlockSize;

        // If there are remaining elements in the left buffer, move them to the right end of the comp() partition.
        // The elements closer to the chunk center are moved first, then the elements at the edges of the array.
        for (std::size_t i = 0; i < left_buffer_size; ++i) {
            this->swap(
                unpartitioned_range_begin - i - 1,
                unpartitioned_range_begin - BlockSize + left_buffer[left_buffer_size - 1 - i]
            );
        }

        // Ditto for the left buffer and the !comp() partition.
        for (std::size_t i = 0; i < right_buffer_size; ++i) {
            this->swap(
                last_item + 1 + i,
                last_item + BlockSize - right_buffer[right_buffer_size - 1 - i]
            );
        }

        unpartitioned_range_begin -= left_buffer_size;
        last_item += right_buffer_size;

        return dynamicBlockPartition(pivot, unpartitioned_range_begin, last_item, left_buffer, right_buffer);
    }


    /*
     * Determines the pivot, creates mini-partitions from the sampled items, and then delegates to the static or dynamic
     * block partitioning methods, depending on the chunk size.
     */
    inline std::size_t selectPivotAndPartitionArray(std::size_t begin, std::size_t end, const std::size_t& k, std::size_t offset) {
        constexpr std::size_t AdaptiveSampleThreshold = 2 * ItemsPerCacheLine;

        std::size_t chunk_size = end - begin;
        std::size_t sample_size = sqrt(chunk_size);

//        printf("%s:%d chunk_size=%lu sample_size=%lu\n", __FILE__, __LINE__, chunk_size, sample_size);

        // If the sample size is smaller than the threshold for adaptive sampling, fall back to median of 3 pivot selection
        // and use dynamic block partitioning.
        if (sample_size < AdaptiveSampleThreshold) {
//            printf("%s:%d\n", __FILE__, __LINE__);
            std::size_t pivot = medianOf3(begin, end);

            return dynamicBlockPartition(pivot, begin + 2, end - 2);
        }

        // The samples are moved to the beginning of the array, and of those samples, the pivot_pos-th largest sample is
        // chosen to be the pivot. The pivot position is determined by the size of k relative to the size of the chunk to partition.
        float pivot_ratio = float(k) / chunk_size;
        std::size_t pivot_pos = pivot_ratio * sample_size;
//        printf("%s:%d pivot_pos=%lu, sample_size=%lu\n", __FILE__, __LINE__, pivot_pos, sample_size);

        // If the pivot position is large enough to make it viable, adjust the pivot position so that the pivot position is
        // the closest power of ItemsPerCacheLine, minus 1. The sample size is also adjusted to maintain the original
        // pivot position : sample size ratio.

        if (pivot_pos >= (AdaptiveSampleThreshold)) {
            // Round the pivot position to the closest power of ItemsPerCacheLine, minus 1
            if (((pivot_pos % ItemsPerCacheLine) >= (ItemsPerCacheLine / 2)) || (pivot_pos < ItemsPerCacheLine)) {
                pivot_pos += ((ItemsPerCacheLine - 1) - (pivot_pos % ItemsPerCacheLine));
            }

            else {
                pivot_pos -= ((pivot_pos % ItemsPerCacheLine) + 1);
            }

            // Account for the offset of the array chunk. For example, if the chunk begins at ItemsPerCacheLine + 3,
            // this will subtract 3 from the pivot position so that the unpartitioned range begins on the start of a cache
            // line.
            if (pivot_pos <= offset) { pivot_pos += (ItemsPerCacheLine - offset); }
            else { pivot_pos -= offset; }

//            printf("%s:%d pivot_pos=%lu, sample_size=%lu\n", __FILE__, __LINE__, pivot_pos, sample_size);

            // Adjust the sample size based on changes to the pivot position.
            if (pivot_pos > 0) {
                sample_size = std::ceil(float(pivot_pos) / pivot_ratio);
            }
        }

//        printf("%s:%d pivot_pos=%lu, sample_size=%lu\n", __FILE__, __LINE__, pivot_pos, sample_size);

        // Use the beginning and end of (sample size / 2) cache lines as the samples.
        for (std::size_t i = 0; i < (sample_size >> 1); ++i) {
            this->swap(begin + (i << 1), begin + (ItemsPerCacheLine * i));
            this->swap(begin + (i << 1) + 1, begin + (ItemsPerCacheLine * i) + (ItemsPerCacheLine - 1));
        }

        // If the sample size is odd, we'll just be lazy and use whatever value is already at begin + sample_size - 1. It's random enough.
//        float* pivot = begin + pivot_pos;

        // Find the pivot_pos-th element of the sampled items.
//        printf("%s:%d calling nth_element begin=%lu end=%lu nth=%lu\n", __FILE__, __LINE__, begin, begin + sample_size, pivot_pos);
        nth_element(begin, begin + sample_size, pivot_pos);

        // Swap all items in the sample slice that are !comp() to the pivot with items at the end of the chunk.
//        std::swap_ranges(pivot + 1, begin + sample_size, end - (sample_size - pivot_pos) + 1);
//        printf("%s:%d %lu %lu %lu\n", __FILE__, __LINE__, begin + pivot_pos + 1, begin + sample_size, end - (sample_size - pivot_pos) + 1);
//        std::swap_ranges(this->nodeAt(begin + pivot_pos + 1), this->nodeAt(begin + sample_size), this->nodeAt(end - (sample_size - pivot_pos) + 1));

        for (auto i = 0; i < this->num_dimensions; ++i) {
            std::swap_ranges(
                this->nodes[i] + (begin + pivot_pos + 1),
                this->nodes[i] + (begin + sample_size),
                this->nodes[i] + (end - (sample_size - pivot_pos) + 1)
            );
        }


        // Partition items between [pivot + 1, !comp() mini partition begin)
        if ((end - begin) - sample_size > (BlockSize << 1u)) {
            return staticBlockPartition(begin + pivot_pos, begin + pivot_pos + 1, end - (sample_size - pivot_pos) + 1);
        }

        else {
            return dynamicBlockPartition(begin + pivot_pos, begin + pivot_pos + 1, end - (sample_size - pivot_pos) + 1);
        }
    }


    // TODO refactor to match signature of std::nth_element
    /*
     * Standard quickselect loop. Narrows the range of items until it finds the nth_element.
     */
    void nth_element(std::size_t begin, std::size_t end, std::size_t n) {
        std::size_t offset = 0;


        while (true) {
//            assert (begin <= end);
//            printf("%s:%d %lu %lu %lu\n", __FILE__, __LINE__, begin, end, n);
//
//            printf("%s:%d ", __FILE__, __LINE__);
//
//            for (long j = 0; j < end - begin; ++j) {
//                printf("[");
//                for (long k = 0; k < num_dimensions; k++) {
//                    printf("%f ", nodes[(j * num_dimensions) + k]);
//                }
//                printf("], ");
//            }
//
//
//            printf("\n");
            if (begin == end) return;

            // If we're looking for the smallest item in the array, find it and move it to the beginning.
            if (n == 0) {
//                printf("%s:%d\n", __FILE__, __LINE__);
                std::size_t min_node_pos = begin;

                for (std::size_t i = begin + 1; i < end; ++i) {
                    if (this->cellAt(i) < this->cellAt(min_node_pos)) {
                        min_node_pos = i;
                    }
                }

                this->swap(begin, min_node_pos);

                return;
            }

            // If we're looking for the largest item in the array, find it and move it to the last item's position.
            else if (n == end - begin - 1) {
//                printf("%s:%d\n", __FILE__, __LINE__);
                std::size_t max_node_pos = begin;

                for (std::size_t i = begin + 1; i < end; ++i) {
                    if (this->cellAt(i) > this->cellAt(max_node_pos)) {
//                        printf("%s:%d %f is new max\n", __FILE__, __LINE__, this->cellAt(i));
                        max_node_pos = i;
                    }
                }

                this->swap(end - 1, max_node_pos);

//                printf("%s:%d ", __FILE__, __LINE__);

//                for (long j = 0; j < end - begin; ++j) {
//                    printf("[");
//                    for (long k = 0; k < num_dimensions; k++) {
//                        printf("%f ", nodes[(j * num_dimensions) + k]);
//                    }
//                    printf("], ");
//                }

                return;
            }

            std::size_t pivot = selectPivotAndPartitionArray(begin, end, n, offset);

//            printf("%s:%d %lu %lu %lu\n", __FILE__, __LINE__, begin, end, n);
//            printf("%s:%d pivot=%lu\n", __FILE__, __LINE__, pivot);

            if (pivot - begin == n) return;

            bool n_is_smaller = n < static_cast<std::size_t>(pivot - begin);



            // If the nth_element is smaller than the pivot's position, set the new search range to [begin, pivot).
            end -= ((end - pivot) & (0 - (n_is_smaller)));

            // If the nth_element is larger than the pivot's position, set the new search range to (pivot, end) and adjust the offset.
            n -= ((pivot - begin + 1) & (0 - (!n_is_smaller)));
            offset += ((pivot - begin + 1) & (0 - (!n_is_smaller)));
            offset &= (ItemsPerCacheLine - 1);
            begin += ((pivot - begin + 1) & (0 - (!n_is_smaller)));

//            printf("%s:%d %lu %lu %lu\n", __FILE__, __LINE__, begin, end, n);
        }
    }
};













