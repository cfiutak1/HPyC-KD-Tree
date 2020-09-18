#include "KDTree.hpp"
#include "../quickselect/AdaptiveCacheAwareBlockquickselect.hpp"

#include <thread>
#include "omp.h"


inline void KDTree::swap(std::size_t index1, std::size_t index2) {
    for (std::size_t i = 0; i < this->num_dimensions; ++i) {
        std::swap(
            this->nodes[i][index1],
            this->nodes[i][index2]
        );
    }
}


inline float* KDTree::pointAt(const std::size_t index) const {
    alignas(32) float* point = new float[this->num_dimensions];

    for (uint64_t i = 0; i < this->num_dimensions; ++i) {
        point[i] = this->nodes[i][index];
    }

    return point;
}


/*
 * Private method that restructures the array given to the constructor to a well-balanced KD-Tree.
 */
void KDTree::buildTree(const uint64_t subarray_begin, const uint64_t subarray_end, unsigned int depth) {
    uint64_t range = subarray_end - subarray_begin;

    // Base case - If there is one element left, it is already sorted and thus in its correct position.
    if (range == 1) { return; }

    // Base case - If there are two elements left, they will be in their correct positions if they are sorted.
    else if (range == 2) {
        if (this->nodes[depth][subarray_begin] > this->nodes[depth][subarray_begin + 1]) {
            this->swap(subarray_begin, subarray_begin + 1);
        }

        return;
    }

    // Partition the current subarray around the median at the current dimension.
    AdaptiveCacheAwareBlockquickselect<> qs(this->nodes, this->num_dimensions, depth);
    qs.nth_element(subarray_begin, subarray_end, (range >> 1u));

    // Build left subtree (all elements left of the median)
    this->buildTree(subarray_begin, subarray_begin + (range >> 1u), (depth + 1) % this->num_dimensions);

    // Build right subtree (all elements right of the median)
    this->buildTree(subarray_begin + (range >> 1u) + 1, subarray_end, (depth + 1) % this->num_dimensions);
}


/*
 * Private method that restructures the array given to the constructor to a well-balanced KD-Tree.
 */
void KDTree::buildTreeParallel(const uint64_t subarray_begin, const uint64_t subarray_end, unsigned int depth, unsigned int num_threads) {
    uint64_t range = subarray_end - subarray_begin;

    // Base case - If there is one element left, it is already sorted and thus in its correct position.
    if (range == 1) { return; }

    // Base case - If there are two elements left, they will be in their correct positions if they are sorted.
    else if (range == 2) {
        if (this->nodes[depth][subarray_begin] > this->nodes[depth][subarray_begin + 1]) {
            this->swap(subarray_begin, subarray_begin + 1);
        }

        return;
    }

    // Partition the current subarray around the median at the current dimension.
    AdaptiveCacheAwareBlockquickselect<> qs(this->nodes, this->num_dimensions, depth);
    qs.nth_element(subarray_begin, subarray_end, (range >> 1u));

    if (num_threads > 0) {
        --num_threads;

        std::thread right_thread(&KDTree::buildTreeParallel, this, subarray_begin + (range >> 1u) + 1, subarray_end, (depth + 1) % this->num_dimensions, num_threads - (num_threads / 2));

        // Build left subtree (all elements left of the median)
        this->buildTreeParallel(subarray_begin, subarray_begin + (range >> 1u), (depth + 1) % this->num_dimensions, num_threads / 2);

        right_thread.join();

        return;
    }

    // Build left subtree (all elements left of the median)
    this->buildTree(subarray_begin, subarray_begin + (range >> 1u), (depth + 1) % this->num_dimensions);

    // Build right subtree (all elements right of the median)
    this->buildTree(subarray_begin + (range >> 1u) + 1, subarray_end, (depth + 1) % this->num_dimensions);
}


/*
 * Public method that returns a priority queue containing the K nearest neighbors to a given query point. Serves as a
 * wrapper for the private recursive nearestNeighborsSearch().
 */
KNNQueue KDTree::nearestNeighborsSearch(const float* query_point, const uint64_t& num_neighbors) const {
    KNNQueue queue(query_point, num_neighbors, this->num_dimensions);

    this->nearestNeighborsSearch(query_point, 0, this->num_points, 0, queue);

    return queue;
}



KNNQueue* KDTree::processQueriesParallel(float** query_points, const uint64_t num_queries, const uint64_t num_neighbors, unsigned int num_threads_in) const {
    KNNQueue* results = new KNNQueue[num_queries];

    #pragma omp parallel for shared(results) num_threads(num_threads_in) schedule(static)
    for (uint64_t i = 0; i < num_queries; ++i) {
        KNNQueue queue(query_points[i], num_neighbors, this->num_dimensions);
        this->nearestNeighborsSearch(query_points[i], 0, this->num_points, 0, queue);
        results[i] = queue;
    }


    return results;
}


/*
 * Private method that executes the K nearest neighbors search for a given query point.
 */
void KDTree::nearestNeighborsSearch(const float* query_point, uint64_t begin, uint64_t end, uint64_t depth, KNNQueue& nearest_neighbors) const {
    uint64_t range = end - begin;
    uint64_t traverser_index = begin + (range >> 1u);

    nearest_neighbors.registerAsNeighborIfCloser(this->pointAt(traverser_index));

    // Base case - If there's one element left, it has already been tested so stop recursing.
    if (range == 1) { return; }

    // Base case - If there's two elements left, the 2nd element has already been tested. Thus, we simply register the
    // 1st element and stop recursing.
    if (range == 2) {
        nearest_neighbors.registerAsNeighborIfCloser(this->pointAt(traverser_index - 1));
        return;
    }

    float query_at_current_dimension = query_point[depth];
    float traverser_at_current_dimension = this->nodes[depth][traverser_index];
    float difference_at_current_dimension = traverser_at_current_dimension - query_at_current_dimension;
    float distance_from_query_at_current_dimension = difference_at_current_dimension * difference_at_current_dimension;

    // If the query is less than the traverser at the current dimension,
    //     1. Traverse down the left subtree.
    //     2. After fully traversing the left subtree, determine if the right subtree can possibly have a closer neighbor.
    //     3. If the right subtree is not viable, return. Otherwise, traverse the right subtree.
    if (query_at_current_dimension < traverser_at_current_dimension) {
        this->nearestNeighborsSearch(query_point, begin, traverser_index, (depth + 1) % this->num_dimensions, nearest_neighbors);

        double farthest_neighbor_distance = nearest_neighbors.top().distance_from_queried_point;

        if (farthest_neighbor_distance < distance_from_query_at_current_dimension) { return; }

        this->nearestNeighborsSearch(query_point, traverser_index + 1, end, (depth + 1) % this->num_dimensions, nearest_neighbors);
    }

    // If the query is greater than or equal to the traverser at the current dimension,
    //     1. Traverse down the right subtree.
    //     2. After fully traversing the right subtree, determine if the left subtree can possibly have a closer neighbor.
    //     3. If the left subtree is not viable, return. Otherwise, traverse the left subtree.
    else {
        this->nearestNeighborsSearch(query_point, traverser_index + 1, end, (depth + 1) % this->num_dimensions, nearest_neighbors);

        double farthest_neighbor_distance = nearest_neighbors.top().distance_from_queried_point;

        if (farthest_neighbor_distance < distance_from_query_at_current_dimension) { return; }

        this->nearestNeighborsSearch(query_point, begin, traverser_index, (depth + 1) % this->num_dimensions, nearest_neighbors);
    }
}
