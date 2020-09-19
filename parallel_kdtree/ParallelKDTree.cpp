#include "ParallelKDTree.hpp"
#include "../quickselect/AdaptiveCacheAwareBlockquickselect.hpp"

#include <thread>
#include <atomic>
#include "omp.h"


/*
 * Private method that restructures the array given to the constructor to a well-balanced KD-Tree.
 */
void ParallelKDTree::buildTree(const uint64_t subarray_begin, const uint64_t subarray_end, unsigned int depth, unsigned int num_threads) {
    uint64_t range = subarray_end - subarray_begin;

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
    AdaptiveCacheAwareBlockquickselect<> qs(this->tree->nodes, this->tree->num_dimensions, depth);
    qs.nth_element(subarray_begin, subarray_end, (range >> 1u));

    if (num_threads > 0) {
        --num_threads;

        std::thread right_thread(&ParallelKDTree::buildTree, this, subarray_begin + (range >> 1u) + 1, subarray_end, (depth + 1) % this->tree->num_dimensions, num_threads - (num_threads / 2));

        // Build left subtree (all elements left of the median)
        this->buildTree(subarray_begin, subarray_begin + (range >> 1u), (depth + 1) % this->tree->num_dimensions, num_threads / 2);

        right_thread.join();

        return;
    }

    // Build left subtree (all elements left of the median)
    this->tree->buildTree(subarray_begin, subarray_begin + (range >> 1u), (depth + 1) % this->tree->num_dimensions);

    // Build right subtree (all elements right of the median)
    this->tree->buildTree(subarray_begin + (range >> 1u) + 1, subarray_end, (depth + 1) % this->tree->num_dimensions);
}




KNNQueue* ParallelKDTree::nearestNeighborsSearches(float** query_points, const uint64_t num_queries, const uint64_t num_neighbors, unsigned int num_threads) const {
    KNNQueue* results = new KNNQueue[num_queries];

#pragma omp parallel for shared(results) num_threads(num_threads) schedule(static)
    for (uint64_t i = 0; i < num_queries; ++i) {
        KNNQueue queue(query_points[i], num_neighbors, this->tree->num_dimensions);
        this->tree->nearestNeighborsSearch(query_points[i], 0, this->tree->num_points, 0, queue);
        results[i] = queue;
    }

    return results;
}


ThreadSafeKNNQueue ParallelKDTree::nearestNeighborsSearch(const float* query_point, const uint64_t num_neighbors) {
    this->nearest_neighbors = {query_point, num_neighbors, this->tree->num_dimensions};

    this->nearestNeighborsSearchTS(query_point, 0, this->tree->num_points, 0, num_neighbors);

    return this->nearest_neighbors;
}

/*
 * Private method that executes the K nearest neighbors search for a given query point.
 */
void ParallelKDTree::nearestNeighborsSearchTS(const float* query_point, uint64_t begin, uint64_t end, const uint64_t depth, const uint64_t num_neighbors) {
    uint64_t range = end - begin;
    uint64_t traverser_index = begin + (range >> 1u);

    nearest_neighbors.registerAsNeighborIfEligible(this->tree->pointAt(traverser_index));

    // Base case - If there's one element left, it has already been tested so stop recursing.
    if (range == 1) { return; }

    // Base case - If there's two elements left, the 2nd element has already been tested. Thus, we simply register the
    // 1st element and stop recursing.
    if (range == 2) {
        nearest_neighbors.registerAsNeighborIfEligible(this->tree->pointAt(traverser_index - 1));
        return;
    }

    float query_at_current_dimension = query_point[depth];
    float traverser_at_current_dimension = this->tree->nodes[depth][traverser_index];
    float difference_at_current_dimension = traverser_at_current_dimension - query_at_current_dimension;
    float distance_from_query_at_current_dimension = difference_at_current_dimension * difference_at_current_dimension;

    // If the query is less than the traverser at the current dimension,
    //     1. Traverse down the left subtree.
    //     2. After fully traversing the left subtree, determine if the right subtree can possibly have a closer neighbor.
    //     3. If the right subtree is not viable, return. Otherwise, traverse the right subtree.
    if (query_at_current_dimension < traverser_at_current_dimension) {
        if (this->available_threads.fetch_sub(1) > 0) {
            std::thread left_thread(&ParallelKDTree::nearestNeighborsSearchTS, this, query_point, begin, traverser_index, (depth + 1) % this->tree->num_dimensions, num_neighbors);

            double farthest_neighbor_distance = nearest_neighbors.queue.top().distance_from_queried_point;

            if (farthest_neighbor_distance < distance_from_query_at_current_dimension) { return; }

            this->nearestNeighborsSearchTS(query_point, traverser_index + 1, end, (depth + 1) % this->tree->num_dimensions, num_neighbors);
            left_thread.join();
            ++this->available_threads;
        }

        else {
            ++this->available_threads;
            this->nearestNeighborsSearchTS(query_point, begin, traverser_index, (depth + 1) % this->tree->num_dimensions, num_neighbors);

            double farthest_neighbor_distance = nearest_neighbors.queue.top().distance_from_queried_point;

            if (farthest_neighbor_distance < distance_from_query_at_current_dimension) { return; }

            this->nearestNeighborsSearchTS(query_point, traverser_index + 1, end, (depth + 1) % this->tree->num_dimensions, num_neighbors);
        }
    }

        // If the query is greater than or equal to the traverser at the current dimension,
        //     1. Traverse down the right subtree.
        //     2. After fully traversing the right subtree, determine if the left subtree can possibly have a closer neighbor.
        //     3. If the left subtree is not viable, return. Otherwise, traverse the left subtree.
    else {
        if (this->available_threads.fetch_sub(1) > 0) {
            std::thread left_thread(&ParallelKDTree::nearestNeighborsSearchTS, this, query_point, traverser_index + 1, end, (depth + 1) % this->tree->num_dimensions, num_neighbors);

            double farthest_neighbor_distance = this->nearest_neighbors.queue.top().distance_from_queried_point;

            if (farthest_neighbor_distance < distance_from_query_at_current_dimension) { return; }

            this->nearestNeighborsSearchTS(query_point, begin, traverser_index, (depth + 1) % this->tree->num_dimensions, num_neighbors);
            left_thread.join();
            ++this->available_threads;
        }

        else {
            ++this->available_threads;
            this->nearestNeighborsSearchTS(query_point, traverser_index + 1, end, (depth + 1) % this->tree->num_dimensions, num_neighbors);

            double farthest_neighbor_distance = nearest_neighbors.queue.top().distance_from_queried_point;

            if (farthest_neighbor_distance < distance_from_query_at_current_dimension) { return; }

            this->nearestNeighborsSearchTS(query_point, begin, traverser_index, (depth + 1) % this->tree->num_dimensions, num_neighbors);
        }
    }
}
