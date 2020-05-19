#ifndef KNNSEARCH_HPP
#define KNNSEARCH_HPP

#include "KDTree.hpp"
#include "KDNode.hpp"
#include "KNNQueue.hpp"


void nearestNeighborsSearchHelper(KDNode* query_point, KDNode* traverser, KNNQueue& queue) {
    if (traverser == nullptr) return;

    queue.registerAsNeighborIfCloser(traverser);

    float query_at_current_dimension = query_point->getCoordinate(traverser->depth);
    float traverser_at_current_dimension = traverser->getCoordinate(traverser->depth);
    float difference_at_current_dimension = traverser_at_current_dimension - query_at_current_dimension;
    float distance_from_query_at_current_dimension = difference_at_current_dimension * difference_at_current_dimension;

    if (query_at_current_dimension < traverser_at_current_dimension) {
        nearestNeighborsSearchHelper(query_point, traverser->left_child, queue);

        double farthest_neighbor_distance = queue.top().distance_from_queried_point;
        if (farthest_neighbor_distance < distance_from_query_at_current_dimension) { return; }

        nearestNeighborsSearchHelper(query_point, traverser->right_child, queue);
    }

    else {
        nearestNeighborsSearchHelper(query_point, traverser->right_child, queue);

        double farthest_neighbor_distance = queue.top().distance_from_queried_point;
        if (farthest_neighbor_distance < distance_from_query_at_current_dimension) { return; }

        nearestNeighborsSearchHelper(query_point, traverser->left_child, queue);
    }

}


KNNQueue* nearestNeighborsSearch(KDNode* query_point, uint64_t num_neighbors, KDTree* tree) {
    KNNQueue* queue = new KNNQueue(query_point, num_neighbors);

    nearestNeighborsSearchHelper(query_point, tree->getRoot(), *queue);

    return queue;
}

#endif