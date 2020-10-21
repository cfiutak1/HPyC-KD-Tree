#include "../kdtree/KDTree.hpp"
#include "../kdtree/KNNQueue.hpp"
#include "../quickselect/AdaptiveCacheAwareBlockquickselect.hpp"


#include <thread>
#include <atomic>
#include "omp.h"


#include <cstdint>

#pragma once

namespace hpyc {


template <typename ItemT>
class ParallelKDTree {
public:
    KDTree<ItemT>* tree;
    unsigned int num_threads;
    KNNQueue<ItemT>* nearest_neighbors;

    void buildTree(const std::size_t subarray_begin, const std::size_t subarray_end, unsigned int depth, unsigned int num_threads);


    ParallelKDTree(ItemT* np_array_in, const std::size_t num_points_in, const std::size_t num_dimensions_in, unsigned int num_threads_in)
    {

        this->tree = new KDTree<ItemT>();
//        this->tree->nodes = nodes_in;
        this->tree->num_points = num_points_in;
        this->tree->num_dimensions = num_dimensions_in;

        this->tree->np_array = new ItemT[num_points_in * num_dimensions_in];

        std::memcpy(this->tree->np_array, np_array_in, sizeof(ItemT) * num_points_in * num_dimensions_in);

        this->tree->nodes = new ItemT*[num_dimensions_in];

        for (std::size_t i = 0; i < num_dimensions_in; ++i) {
            this->tree->nodes[i] = np_array_in + (i * num_points_in);
        }

        this->buildTree(0, num_points_in, 0, num_threads_in);
    }

    // TODO It might be worth having fields for these instead in the ParallelKDTree class as well
    ParallelKDTree(float** nodes_in, const std::size_t num_points_in, std::size_t num_dimensions_in, unsigned int num_threads_in):
        num_threads(num_threads_in)
    {
        this->tree = new KDTree<ItemT>();
        this->tree->nodes = nodes_in;
        this->tree->num_points = num_points_in;
        this->tree->num_dimensions = num_dimensions_in;

        this->buildTree(0, num_points_in, 0, num_threads_in);
    }

    ~ParallelKDTree() {
        delete this->tree;
    }

    void nearestNeighborsSearches(const ItemT* query_points, const std::size_t num_queries, const std::size_t num_neighbors, std::size_t* indices, double* distances) const;

};

/*
 * Private method that restructures the array given to the constructor to a well-balanced KD-Tree.
 */
template <typename ItemT>
void ParallelKDTree<ItemT>::buildTree(const std::size_t subarray_begin, const std::size_t subarray_end, unsigned int depth, unsigned int num_threads) {
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

    else if (range == 3) {
        AdaptiveCacheAwareBlockquickselect<ItemT> qs(this->tree->nodes, this->tree->num_dimensions, depth);
        qs.sort3(subarray_begin);

        return;
    }

    // Partition the current subarray around the median at the current dimension.
    AdaptiveCacheAwareBlockquickselect<ItemT, 64> qs(this->tree->nodes, this->tree->num_dimensions, depth);
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

// nearestNeighborsSearch(ItemT* query_point, const std::size_t num_neighbors, std::size_t* indices, double* distances)
template <typename ItemT>
void ParallelKDTree<ItemT>::nearestNeighborsSearches(const ItemT* query_points, const std::size_t num_queries, const std::size_t num_neighbors, std::size_t* indices, double* distances) const {
#pragma omp parallel for shared(query_points, indices, distances) num_threads(num_threads) schedule(static)
    for (std::size_t i = 0; i < num_queries; ++i) {
        std::size_t write_index = i * num_neighbors;

        this->tree->nearestNeighborsSearch(query_points + (i * this->tree->num_dimensions), num_neighbors, indices + write_index, distances + write_index);
    }
}

}