#include "SingleQuerySearcher.hpp"


KNNQueue* SingleQuerySearcher::nearestNeighborsSearch() {
    this->nearestNeighborsSearchHelper(this->tree->getRoot(), 0);

    return this->queue;
}


void SingleQuerySearcher::nearestNeighborsSearchHelper(KDNode* traverser, uint64_t depth) {
    if (traverser == nullptr) return;

    this->queue->registerAsNeighborIfCloser(traverser);

    float query_at_current_dimension = this->query_point->getCoordinate(depth);
    float traverser_at_current_dimension = traverser->getCoordinate(depth);
    float distance_from_query_at_dimension = pow(traverser_at_current_dimension - query_at_current_dimension, 2);

    if (query_at_current_dimension < traverser_at_current_dimension) {
        this->nearestNeighborsSearchHelper(traverser->left_child, depth + 1);
        float farthest_neighbor_dimension_query_distance = this->queue->top().distance_from_queried_point;

        if (farthest_neighbor_dimension_query_distance < distance_from_query_at_dimension) {
            return;
        }

        this->nearestNeighborsSearchHelper(traverser->right_child, depth + 1);
    }

    else {
        this->nearestNeighborsSearchHelper(traverser->right_child, depth + 1);
        float farthest_neighbor_dimension_query_distance = this->queue->top().distance_from_queried_point;

        if (farthest_neighbor_dimension_query_distance < distance_from_query_at_dimension) {
            return;
        }

        this->nearestNeighborsSearchHelper(traverser->left_child, depth + 1);
    }
}
