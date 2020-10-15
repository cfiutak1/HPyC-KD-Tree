#include "ParallelKDTree.hpp"
#include "../quickselect/AdaptiveCacheAwareBlockquickselect.hpp"


#include <thread>
#include <atomic>
#include "omp.h"


/*
 * Private method that restructures the array given to the constructor to a well-balanced KD-Tree.
 */
void ParallelKDTree::buildTree(const std::size_t subarray_begin, const std::size_t subarray_end, unsigned int depth, unsigned int num_threads) {
    std::size_t range = subarray_end - subarray_begin;

    // Base case - If there is one element left, it is already sorted and thus in its correct position.
    if (range == 1) { return; }

    // Base case - If there are two elements left, they will be in their correct positions if they are sorted.
    else if (range == 2) {
        if (this->tree->nodes[depth][subarray_begin] > this->tree->nodes[depth][subarray_begin + 1]) {
            this->tree->swap(subarray_begin, subarray_begin + 1);
        }

        return;
    }

    // Partition the current subarray around the median at the current dimension.
    AdaptiveCacheAwareBlockquickselect<64> qs(this->tree->nodes, this->tree->num_dimensions, depth);
    qs.nth_element(subarray_begin, subarray_end, (range / 2));

    unsigned int new_depth = (depth + 1) & (-1 + (depth == this->tree->num_dimensions - 1));

    if (num_threads > 0) {
        --num_threads;

        std::thread right_thread(&ParallelKDTree::buildTree, this, subarray_begin + (range / 2) + 1, subarray_end, new_depth, num_threads - (num_threads / 2));

        // Build left subtree (all elements left of the median)
        this->buildTree(subarray_begin, subarray_begin + (range / 2), new_depth, num_threads / 2);

        right_thread.join();

        return;
    }

    // Build left subtree (all elements left of the median)
    this->tree->buildTree(subarray_begin, subarray_begin + (range / 2), new_depth);

    // Build right subtree (all elements right of the median)
    this->tree->buildTree(subarray_begin + (range / 2) + 1, subarray_end, new_depth);
}


KNNQueue* ParallelKDTree::nearestNeighborsSearches(float** query_points, const std::size_t num_queries, const std::size_t num_neighbors, unsigned int num_threads) const {
    KNNQueue* queues = new KNNQueue[num_threads];
//
//#pragma omp parallel for shared(results) num_threads(num_threads) schedule(static)
//    for (std::size_t i = 0; i < num_queries; ++i) {
//        KNNQueue queue(query_points[i], num_neighbors, this->tree->num_dimensions);
//        this->tree->nearestNeighborsSearch(query_points[i], 0, this->tree->num_points, 0, queue);
//        results[i] = queue;
//    }
//
//    return results;
}
