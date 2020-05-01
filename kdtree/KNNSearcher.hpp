#ifndef KNNSEARCHER_HPP
#define KNNSEARCHER_HPP

// #include "../construction_thread_pool/KDTree.hpp"
#include "../kdtree/KDTree.hpp"
#include "KNNQueue.hpp"
#include "../filedata/QueryFileData.hpp"

class KNNSearcher {
private:
    KDTree* tree;
    uint64_t num_dimensions;
    uint64_t num_neighbors_to_return;
    Point* query_point;
    KNNQueue* queue;
    // unsigned int prune_count = 0;

public:
    KNNSearcher(KDTree* tree_in, uint64_t num_neighbors_to_return_in, Point* query_point_in):
        tree(tree_in),
        num_dimensions(tree_in->getNumDimensions()),
        num_neighbors_to_return(num_neighbors_to_return_in),
        query_point(query_point_in)
    {
        this->queue = new KNNQueue(query_point_in, num_neighbors_to_return_in);
    }

    KNNSearcher(KDTree* tree_in, QueryFileData* file_data, Point* query_point_in):
        tree(tree_in),
        num_dimensions(file_data->num_dimensions),
        num_neighbors_to_return(file_data->num_neighbors_to_return),
        query_point(query_point_in)
    {
        this->queue = new KNNQueue(query_point_in, file_data->num_neighbors_to_return);
    }

    // ~KNNSearcher() {
    //     delete this->queue;
    // }

    KNNQueue* nearestNeighborsSearch();
    void nearestNeighborsSearchHelper(KDNode* traverser, uint64_t depth);
};

#endif
