#include "KNNQueue.hpp"
#include "../filedata/TrainingFileData.hpp"
#include "../quickselect/AdaptiveCacheAwareBlockquickselect.hpp"

#include <cstdint>
#include <cstring>

#pragma once

namespace hpyc {

template <typename ItemT>
class KDTree {
public:
    alignas(64) ItemT* np_array;
    alignas(64) ItemT** nodes;

    void buildTree(const std::size_t subarray_begin, const std::size_t subarray_end, const unsigned int depth);

    template<bool CheckIfFull=true>
    void nearestNeighborsSearch(const ItemT* query_point, std::size_t begin, std::size_t end, std::size_t depth, KNNQueue<ItemT>& nearest_neighbors) const;

    inline void swap(std::size_t index1, std::size_t index2) {
        for (std::size_t i = 0; i < this->num_dimensions; ++i) {
            std::swap(
                    this->nodes[i][index1],
                    this->nodes[i][index2]
            );
        }
    }


    std::size_t num_points;
    std::size_t num_dimensions;

    KDTree() = default;

    KDTree(ItemT* np_array_in, const std::size_t num_points_in, const std::size_t num_dimensions_in):
        num_points(num_points_in),
        num_dimensions(num_dimensions_in)
    {
        this->np_array = new ItemT[num_points_in * num_dimensions_in];

        std::memcpy(this->np_array, np_array_in, sizeof(ItemT) * num_points_in * num_dimensions_in);

        // TODO get rid of this copy
        this->nodes = new ItemT*[num_dimensions_in];

        for (std::size_t i = 0; i < num_dimensions_in; ++i) {
            this->nodes[i] = np_array_in + (i * num_points_in);
        }

        this->buildTree(0, num_points_in, 0);
    }


    KDTree(ItemT** nodes_in, const std::size_t num_points_in, const std::size_t num_dimensions_in):
        nodes(nodes_in),
        num_points(num_points_in),
        num_dimensions(num_dimensions_in)
    {
        this->buildTree(0, num_points_in, 0);
    }


    ~KDTree() {
        delete[] this->nodes;
        delete[] this->np_array;
    }

    inline void readPointAt(ItemT* point, const std::size_t index) const {
        for (std::size_t i = 0; i < this->num_dimensions; ++i) {
            point[i] = this->nodes[i][index];
        }
    }

    void nearestNeighborsSearch(const ItemT* query_point, const std::size_t num_neighbors, std::size_t* indices, double* distances) const;
};


/*
 * Private method that builds an implicit KD tree for the given range. The root of a given subtree is always placed at
 * the median position.
 */
template <typename ItemT>
void KDTree<ItemT>::buildTree(const std::size_t subarray_begin, const std::size_t subarray_end, const unsigned int depth) {
    std::size_t range = subarray_end - subarray_begin;

    // Base case - If there is one element left, it is already sorted and thus in its correct position.
    if (range == 1) { return; }

    // Base case - If there are two elements left, they will be in their correct positions if they are sorted.
    else if (range == 2) {
        if (this->nodes[depth][subarray_begin] > this->nodes[depth][subarray_begin + 1]) {
            this->swap(subarray_begin, subarray_begin + 1);
        }

        return;
    }

    else if (range == 3) {
        AdaptiveCacheAwareBlockquickselect<ItemT> qs(this->nodes, this->num_dimensions, depth);
        qs.sort3(subarray_begin);

        return;
    }

    // Partition the current subarray around the median at the current dimension.
    AdaptiveCacheAwareBlockquickselect<ItemT> qs(this->nodes, this->num_dimensions, depth);
    qs.nth_element(subarray_begin, subarray_end, (range / 2));

    unsigned int new_depth = (depth + 1) & (-1 + (depth == this->num_dimensions - 1));

    // Build left subtree (all elements left of the median)
    this->buildTree(subarray_begin, subarray_begin + (range / 2), new_depth);

    // Build right subtree (all elements right of the median)
    this->buildTree(subarray_begin + (range / 2) + 1, subarray_end, new_depth);
}


/*
 * Public method that returns a priority queue containing the K nearest neighbors to a given query point. Serves as a
 * wrapper for the private recursive nearestNeighborsSearch().
 */
template <typename ItemT>
void KDTree<ItemT>::nearestNeighborsSearch(const ItemT* query_point, const std::size_t num_neighbors, std::size_t* indices, double* distances) const {
    KNNQueue<ItemT> queue(query_point, num_neighbors, this->num_dimensions);

    this->nearestNeighborsSearch(query_point, 0, this->num_points, 0, queue);
        queue.sortArray();

    for (std::size_t i = num_neighbors; i > 0; --i) {
        indices[i - 1] = queue.array[i - 1].index;
        distances[i - 1] = queue.array[i - 1].distance_from_queried_point;
    }
}


/*
 * Private method that searches for the K nearest neighbors to a given query_point. Recurses until there are no more
 * points to compare to, pruning any branches that are no longer feasible.
 */
template <typename ItemT>
template <bool CheckIfFull>
void KDTree<ItemT>::nearestNeighborsSearch(const ItemT* query_point, std::size_t begin, std::size_t end, std::size_t depth, KNNQueue<ItemT>& nearest_neighbors) const {
    std::size_t range = end - begin;
    std::size_t traverser_index = begin + (range / 2);

    this->readPointAt(nearest_neighbors.getPotentialNeighbor(), traverser_index);

    // Optimization: This templatization is done to prevent unnecessarily checking if the KNN queue is full. Once
    // the queue is full, it will never be not full again, so we can cut down on some work by only checking the fullness
    // until it becomes full.
    // TODO - Can probably cut down on code reuse by making this template argument a function pointer instead of a boolean.
    if constexpr (CheckIfFull) {
        if (nearest_neighbors.full()) {
            nearest_neighbors.heapify();
            return this->nearestNeighborsSearch<false>(query_point, begin, end, depth, nearest_neighbors);
        }

        nearest_neighbors.registerAsNeighbor(traverser_index);

        // Base case - If there's two elements left, the 2nd element has already been tested. Thus, we simply register the
        // 1st element and stop recursing.
        if (range == 2) {
            this->readPointAt(nearest_neighbors.getPotentialNeighbor(), traverser_index - 1);

            if (nearest_neighbors.full()) {
                nearest_neighbors.registerAsNeighborIfCloser(traverser_index);
            }

            else {
                nearest_neighbors.registerAsNeighbor(traverser_index);
            }

            return;
        }
    }

    else {
        nearest_neighbors.registerAsNeighborIfCloser(traverser_index);

        // Base case - If there's two elements left, the 2nd element has already been tested. Thus, we simply register the
        // 1st element and stop recursing.
        if (range == 2) {
            this->readPointAt(nearest_neighbors.getPotentialNeighbor(), traverser_index - 1);

            nearest_neighbors.registerAsNeighborIfCloser(traverser_index);
            return;
        }
    }

    // Base case - If there's one element left, it has already been tested so stop recursing.
    if (range == 1) { return; }

    // TODO - These used to be floats. If there's very very small distances, this might behave differently if ItemT is a double instead.
    ItemT query_at_current_dimension = query_point[depth];
    ItemT traverser_at_current_dimension = this->nodes[depth][traverser_index];
    ItemT difference_at_current_dimension = traverser_at_current_dimension - query_at_current_dimension;
    ItemT distance_from_query_at_current_dimension = difference_at_current_dimension * difference_at_current_dimension;

    unsigned int new_depth = (depth + 1) & (-1 + (depth == this->num_dimensions - 1));

    // If the query is less than the traverser at the current dimension,
    //     1. Traverse down the left subtree.
    //     2. After fully traversing the left subtree, determine if the right subtree can possibly have a closer neighbor.
    //     3. If the right subtree is not viable, return. Otherwise, traverse the right subtree.
    if (query_at_current_dimension < traverser_at_current_dimension) {
        this->nearestNeighborsSearch<CheckIfFull>(query_point, begin, traverser_index, new_depth, nearest_neighbors);

        double farthest_neighbor_distance = nearest_neighbors.top().distance_from_queried_point;

        if (farthest_neighbor_distance < distance_from_query_at_current_dimension) { return; }

        this->nearestNeighborsSearch<CheckIfFull>(query_point, traverser_index + 1, end, new_depth, nearest_neighbors);
    }

    // If the query is greater than or equal to the traverser at the current dimension,
    //     1. Traverse down the right subtree.
    //     2. After fully traversing the right subtree, determine if the left subtree can possibly have a closer neighbor.
    //     3. If the left subtree is not viable, return. Otherwise, traverse the left subtree.
    else {
        this->nearestNeighborsSearch<CheckIfFull>(query_point, traverser_index + 1, end, new_depth, nearest_neighbors);

        double farthest_neighbor_distance = nearest_neighbors.top().distance_from_queried_point;

        if (farthest_neighbor_distance < distance_from_query_at_current_dimension) { return; }

        this->nearestNeighborsSearch<CheckIfFull>(query_point, begin, traverser_index, new_depth, nearest_neighbors);
    }
}
}


