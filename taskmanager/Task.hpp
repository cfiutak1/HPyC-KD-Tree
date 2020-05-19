#ifndef TASK_HPP
#define TASK_HPP

#include "../kdtree/KDTree.hpp"
#include "../kdtree/KNNQueue.hpp"
//#include "../kdtree/KNNSearcher.hpp"
#include "../kdtree/KNNSearch.hpp"


// TODO this order is horrible design
template <typename ReturnType, typename ArgType>
class Task {
public:
    virtual ReturnType* operator()(ArgType*) { return nullptr; }
};


class QueryTask : public Task<KNNQueue, KDNode> {
public:
    KDTree* tree;
    const uint64_t& num_neighbors;

    QueryTask(KDTree* tree_in, const uint64_t& num_neighbors_in): tree(tree_in), num_neighbors(num_neighbors_in) {}

    KNNQueue* operator()(KDNode* query_point) override {
        return nearestNeighborsSearch(query_point, this->num_neighbors, this->tree);
    }

};

#endif
