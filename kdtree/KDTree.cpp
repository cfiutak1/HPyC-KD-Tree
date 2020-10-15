#include "KDTree.hpp"
#include "../quickselect/AdaptiveCacheAwareBlockquickselect.hpp"

namespace hpyc {

/*
 * Private method that builds an implicit KD tree for the given range. The root of a given subtree is always placed at
 * the median position.
 */
void KDTree::buildTree(const std::size_t subarray_begin, const std::size_t subarray_end, const unsigned int depth) {
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
        AdaptiveCacheAwareBlockquickselect<> qs(this->nodes, this->num_dimensions, depth);
        qs.sort3(subarray_begin);

        return;
    }

    // Partition the current subarray around the median at the current dimension.
    AdaptiveCacheAwareBlockquickselect<64> qs(this->nodes, this->num_dimensions, depth);
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
void KDTree::nearestNeighborsSearch(const float* query_point, KNNQueue& queue) const {
    this->nearestNeighborsSearch(query_point, 0, this->num_points, 0, queue);

    queue.validate();
}


/*
 * Private method that searches for the K nearest neighbors to a given query_point. Recurses until there are no more
 * points to compare to, pruning any branches that are no longer feasible.
 */
template <bool CheckIfFull>
void KDTree::nearestNeighborsSearch(const float* query_point, std::size_t begin, std::size_t end, std::size_t depth, KNNQueue& nearest_neighbors) const {
    std::size_t range = end - begin;
    std::size_t traverser_index = begin + (range >> 1u);

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

        nearest_neighbors.registerAsNeighbor();

        // Base case - If there's two elements left, the 2nd element has already been tested. Thus, we simply register the
        // 1st element and stop recursing.
        if (range == 2) {
            this->readPointAt(nearest_neighbors.getPotentialNeighbor(), traverser_index - 1);

            if (nearest_neighbors.full()) {
                nearest_neighbors.registerAsNeighborIfCloser();
            }

            else {
                nearest_neighbors.registerAsNeighbor();
            }

            return;
        }
    }

    else {
        nearest_neighbors.registerAsNeighborIfCloser();

        // Base case - If there's two elements left, the 2nd element has already been tested. Thus, we simply register the
        // 1st element and stop recursing.
        if (range == 2) {
            this->readPointAt(nearest_neighbors.getPotentialNeighbor(), traverser_index - 1);

            nearest_neighbors.registerAsNeighborIfCloser();
            return;
        }
    }

    // Base case - If there's one element left, it has already been tested so stop recursing.
    if (range == 1) { return; }

    float query_at_current_dimension = query_point[depth];
    float traverser_at_current_dimension = this->nodes[depth][traverser_index];
    float difference_at_current_dimension = traverser_at_current_dimension - query_at_current_dimension;
    float distance_from_query_at_current_dimension = difference_at_current_dimension * difference_at_current_dimension;

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