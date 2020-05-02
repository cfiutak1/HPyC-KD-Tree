#include "SingleQueryWorker.hpp"
#include <cmath>

void SingleQueryWorker::doWork() {
    KDNode* node_to_explore = nullptr;

    while (node_to_explore == nullptr && this->searcher->active_threads != 0) {
        node_to_explore = this->frontier_node_queue->requestNode();
    }

    do {
        this->searcher->registerActiveThread();
        this->traverseWhileViable(node_to_explore);
        this->searcher->deregisterActiveThread();

        node_to_explore = this->frontier_node_queue->requestNode();
    } while (node_to_explore != nullptr);

    this->searcher->registerCompletedThread();
}


void SingleQueryWorker::traverseWhileViable(KDNode* traverser) {
    if (traverser == nullptr) { return; }

    this->nearest_neighbors->registerAsNeighborIfCloserTS(traverser);

    float query_at_current_dimension = this->searcher->query_point->getCoordinate(traverser->depth);
    float traverser_at_current_dimension = traverser->getCoordinate(traverser->depth);

    float distance_from_query_at_dimension = pow(traverser_at_current_dimension - query_at_current_dimension, 2);

    if (query_at_current_dimension < traverser_at_current_dimension) {
        this->traverseWhileViable(traverser->left_child);

        double farthest_neighbor_dimension_query_distance = this->nearest_neighbors->top().distance_from_queried_point;

        if (farthest_neighbor_dimension_query_distance < distance_from_query_at_dimension) { return; }

        if (traverser->right_child != nullptr) this->frontier_node_queue->enqueue(traverser->right_child);
    }

    else {
        this->traverseWhileViable(traverser->right_child);

        double farthest_neighbor_dimension_query_distance = this->nearest_neighbors->top().distance_from_queried_point;

        if (farthest_neighbor_dimension_query_distance < distance_from_query_at_dimension) { return; }

        if (traverser->left_child != nullptr) this->frontier_node_queue->enqueue(traverser->left_child);
    }
}
