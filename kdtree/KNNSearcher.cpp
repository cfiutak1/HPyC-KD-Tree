#include "KNNSearcher.hpp"
#include <cmath>


KNNQueue* KNNSearcher::nearestNeighborsSearch() {
    this->nearestNeighborsSearchHelper(this->tree->getRoot(), 0);
    // printf("%s:%d Pruned %u nodes\n", __FILE__, __LINE__, this->prune_count);
    return this->queue;
}


void KNNSearcher::nearestNeighborsSearchHelper(KDNode* traverser, uint64_t depth) {
    // printf("%s:%d\n", __FILE__, __LINE__);
    if (traverser == nullptr) return;

    this->queue->registerAsNeighborIfCloser(traverser->point);

    // printf("%s:%d num_dimensions=%llu\n", __FILE__, __LINE__, this->num_dimensions);

    uint64_t current_dimension = depth % num_dimensions;
    float query_at_current_dimension = this->query_point->getCoordinate(current_dimension);
    float traverser_at_current_dimension = traverser->point->getCoordinate(current_dimension);
    float distance_from_query_at_dimension = pow(traverser_at_current_dimension - query_at_current_dimension, 2);

    if (query_at_current_dimension < traverser_at_current_dimension) {
        this->nearestNeighborsSearchHelper(traverser->left_child, depth + 1);
        float farthest_neighbor_dimension_query_distance = this->queue->top().distance_from_queried_point;

        // printf("%s:%d %f < %f?\n", __FILE__, __LINE__, farthest_neighbor_dimension_query_distance, distance_from_query_at_dimension);

        if (farthest_neighbor_dimension_query_distance < distance_from_query_at_dimension) {
            // printf("%s:%d Pruning\n", __FILE__, __LINE__);
            // this->prune_count++;
            return;
        }

        this->nearestNeighborsSearchHelper(traverser->right_child, depth + 1);
    }

    else {
        this->nearestNeighborsSearchHelper(traverser->right_child, depth + 1);
        float farthest_neighbor_dimension_query_distance = this->queue->top().distance_from_queried_point;
        // printf("%s:%d %f < %f?\n", __FILE__, __LINE__, farthest_neighbor_dimension_query_distance, distance_from_query_at_dimension);

        if (farthest_neighbor_dimension_query_distance < distance_from_query_at_dimension) {
            // printf("%s:%d Pruning\n", __FILE__, __LINE__);
            // this->prune_count++;
            return;
        }

        this->nearestNeighborsSearchHelper(traverser->left_child, depth + 1);
    }
}
