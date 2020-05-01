#ifndef SINGLEQUERYWORKER_HPP
#define SINGLEQUERYWORKER_HPP

#include "KNNSingleQuerySearcher.hpp"
#include "FrontierNodeQueue.hpp"
#include "../kdtree/KNNQueue.hpp"

// class KNNSingleQuerySearcher;

class SingleQueryWorker {
private:
    KNNSingleQuerySearcher* searcher;
    KNNQueue* nearest_neighbors;
    FrontierNodeQueue* frontier_node_queue;

public:
    unsigned int id = 0;

    SingleQueryWorker(KNNSingleQuerySearcher* searcher_in):
        searcher(searcher_in),
        nearest_neighbors(searcher_in->nearest_neighbors),
        frontier_node_queue(searcher_in->frontier_node_queue)
    {}

    void doWork();
    void traverseWhileViable(KDNode* traverser);
};

#endif
