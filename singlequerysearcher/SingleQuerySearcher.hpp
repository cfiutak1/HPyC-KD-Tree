#ifndef SINGLEQUERYSEARCHER_HPP
#define SINGLEQUERYSEARCHER_HPP

#include <cstdint>
#include "../kdtree/Point.hpp"
#include "../kdtree/KDTree.hpp"
#include "../kdtree/KNNQueue.hpp"

class SingleQuerySearcher {
private:
private:
    KDTree* tree;
    uint64_t num_dimensions;
    uint64_t num_neighbors_to_return;
    Point* query_point;
    KNNQueue* queue;
    // unsigned int prune_count = 0;

public:
    SingleQuerySearcher(KDTree* tree_in, uint64_t num_neighbors_to_return_in, Point* query_point_in):
        tree(tree_in),
        num_dimensions(tree_in->getNumDimensions()),
        num_neighbors_to_return(num_neighbors_to_return_in),
        query_point(query_point_in)
    {
        this->queue = new KNNQueue(query_point_in, num_neighbors_to_return_in);
    }


    KNNQueue* nearestNeighborsSearch();
    void nearestNeighborsSearchHelper(KDNode* traverser, uint64_t depth);

};

#endif
