#ifndef KNNSEARCHER_HPP
#define KNNSEARCHER_HPP

#include "KDTree.hpp"
#include "KNNQueue.hpp"
#include "../filedata/QueryFileData.hpp"

class KNNSearcher {
private:
    KDTree* tree;
    uint64_t num_dimensions;
    uint64_t num_neighbors_to_return;
    KDNode* query_point;
    KNNQueue* queue;

public:
    KNNSearcher(KDTree* tree_in, uint64_t num_neighbors_to_return_in, KDNode* query_point_in):
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
