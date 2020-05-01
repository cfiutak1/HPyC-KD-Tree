#ifndef KNNSINGLEQUERYSEARCHER_HPP
#define KNNSINGLEQUERYSEARCHER_HPP

#include "../kdtree/KDTree.hpp"
#include "../kdtree/KNNQueue.hpp"
#include "FrontierNodeQueue.hpp"
#include "../filedata/QueryFileData.hpp"

#include <cmath>

class SingleQueryWorker;


class KNNSingleQuerySearcher {
public:
    std::vector<SingleQueryWorker*> workers;
    std::atomic<long> potential_leaves_to_visit = 0;
    std::atomic<unsigned int> completed_threads = 0;
    std::atomic<unsigned int> active_threads = 0;
    KDTree* tree;
    uint64_t num_dimensions = 0;
    Point* query_point;
    uint64_t num_neighbors = 0;
    unsigned int num_threads = 0;
    KNNQueue* nearest_neighbors;
    FrontierNodeQueue* frontier_node_queue;
    std::atomic<unsigned int> pruned_nodes = 0;
    bool completed = false;

    KNNSingleQuerySearcher(KDTree* tree_in, Point* query_point_in, uint64_t num_neighbors_in, unsigned int num_threads_in):
        tree(tree_in),
        num_dimensions(tree_in->getNumDimensions()),
        query_point(query_point_in),
        num_neighbors(num_neighbors_in),
        num_threads(num_threads_in)
    {
        this->nearest_neighbors = new KNNQueue(query_point_in, num_neighbors_in);
        this->potential_leaves_to_visit = pow(2, this->tree->getHeight() - 1);
        this->workers.reserve(num_threads_in);

        this->frontier_node_queue = new FrontierNodeQueue(this->tree->getRoot());
    }


    inline void registerCompletedThread() {
        this->completed_threads++;
        if (this->completed_threads == this->num_threads) this->completed = true;
    }

    inline void registerActiveThread() { ++this->completed_threads; }
    inline void deregisterActiveThread() { --this->completed_threads; }

    KNNQueue* nearestNeighborsSearch();
    void nearestNeighborsSearchHelper(KDNode* traverser, uint64_t depth);

    void scheduleWorkers();
    void waitUntilComplete();

    bool keepWorking() {
        return this->frontier_node_queue->tasksRemain() || this->active_threads > 0;
    }
};

#endif
