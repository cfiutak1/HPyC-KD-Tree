#ifndef TASK_HPP
#define TASK_HPP

#include "../kdtree/KDTree.hpp"
#include "../kdtree/KNNQueue.hpp"
#include "../kdtree/Point.hpp"
#include "../kdtree/KNNSearcher.hpp"

#include <cassert>

template <typename ReturnType, typename ArgType>
class Task {
public:
    ReturnType* operator()(ArgType*) { return nullptr; }
};


class QueryTask : public Task<KNNQueue, Point> {
public:
    KDTree* tree;
    const uint64_t& num_neighbors;

    QueryTask(KDTree* tree_in, const uint64_t& num_neighbors_in): tree(tree_in), num_neighbors(num_neighbors_in) {}

    KNNQueue* operator()(Point* query_point) {
        KNNSearcher searcher(this->tree, this->num_neighbors, query_point);
//        assert(tree != nullptr);
//        assert(query_point != nullptr);
        return searcher.nearestNeighborsSearch();
    }

};

#endif
