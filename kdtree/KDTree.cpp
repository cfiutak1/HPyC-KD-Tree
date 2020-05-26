#include "KDTree.hpp"


void KDTree::buildTree() {
    uint64_t begin = 0;
    uint64_t end = this->nodes.size();
    this->buildSubTree(begin, end, 0);
}


void KDTree::buildSubTree(const uint64_t& begin, const uint64_t& end, uint64_t depth) {
    uint64_t range = end - begin;

    if (range == 1) { return; }

    else if (range == 2) {
        if (this->nodes[begin][depth % this->num_dimensions] > this->nodes[begin + 1][depth % this->num_dimensions]) {
            std::swap(this->nodes[begin], this->nodes[begin + 1]);
        }

        return;
    }

    uint64_t median = range / 2;

    this->selectors[depth % this->num_dimensions]->adaptiveQuickselect(
        this->nodes.begin() + begin,
        this->nodes.begin() + end,
        median
    );


    uint64_t l_begin = begin;
    uint64_t l_end = l_begin + median;

    uint64_t r_begin = begin + median + 1;
    uint64_t r_end = end;

    this->buildSubTree(l_begin, l_end, depth + 1);
    this->buildSubTree(r_begin, r_end, depth + 1);
}


KNNQueue* KDTree::nearestNeighborsSearch(float* query_point, uint64_t num_neighbors) {
    KNNQueue* queue = new KNNQueue(query_point, num_neighbors, this->num_dimensions);

    this->nearestNeighborsSearchHelper(query_point, 0, this->num_points, 0, *queue);

    return queue;
}


void KDTree::nearestNeighborsSearchHelper(float* query_point, uint64_t begin, uint64_t end, uint64_t depth, KNNQueue& queue) {
    uint64_t range = end - begin;
    uint64_t median = range / 2;
    uint64_t traverser_index = begin + median;

    queue.registerAsNeighborIfCloser(this->nodes[traverser_index]);

    if (range == 1) { return; }

    if (range == 2) {
        queue.registerAsNeighborIfCloser(this->nodes[traverser_index - 1]);
        return;
    }

    float query_at_current_dimension = query_point[depth % this->num_dimensions];
    float traverser_at_current_dimension = this->nodes[traverser_index][depth % this->num_dimensions];
    float difference_at_current_dimension = traverser_at_current_dimension - query_at_current_dimension;
    float distance_from_query_at_current_dimension = difference_at_current_dimension * difference_at_current_dimension;

    if (query_at_current_dimension < traverser_at_current_dimension) {
        uint64_t l_begin = begin;
        uint64_t l_end = traverser_index;

        nearestNeighborsSearchHelper(query_point, l_begin, l_end, depth + 1, queue);

        double farthest_neighbor_distance = queue.top().distance_from_queried_point;

        uint64_t r_begin = traverser_index + 1;
        uint64_t r_end = end;

        if (farthest_neighbor_distance < distance_from_query_at_current_dimension) { return; }

        nearestNeighborsSearchHelper(query_point, r_begin, r_end, depth + 1, queue);
    }

    else {
        uint64_t r_begin = traverser_index + 1;
        uint64_t r_end = end;

        nearestNeighborsSearchHelper(query_point, r_begin, r_end, depth + 1, queue);

        double farthest_neighbor_distance = queue.top().distance_from_queried_point;

        uint64_t l_begin = begin;
        uint64_t l_end = traverser_index;

        if (farthest_neighbor_distance < distance_from_query_at_current_dimension) { return; }

        nearestNeighborsSearchHelper(query_point, l_begin, l_end, depth + 1, queue);
    }
}
