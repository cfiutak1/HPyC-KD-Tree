//#include "ThreadSafeKNNQueue.hpp"
#include "../kdtree/KDTree.hpp"
#include "../kdtree/KNNQueue.hpp"

#include <cstdint>

#pragma once


class ParallelKDTree {
private:
    KDTree* tree;
    unsigned int num_threads;
    KNNQueue* nearest_neighbors;

    void buildTree(const std::size_t subarray_begin, const std::size_t subarray_end, unsigned int depth, unsigned int num_threads);

public:
    // TODO It might be worth having fields for these instead in the ParallelKDTree class as well
    ParallelKDTree(float** nodes_in, const std::size_t num_points_in, std::size_t num_dimensions_in, unsigned int num_threads_in):
        num_threads(num_threads_in)
    {
        this->tree = new KDTree();
        this->tree->nodes = nodes_in;
        this->tree->num_points = num_points_in;
        this->tree->num_dimensions = num_dimensions_in;

        this->buildTree(0, num_points_in, 0, num_threads_in);
    }

    ~ParallelKDTree() {
        delete this->tree;
    }

    KNNQueue* nearestNeighborsSearches(const float** query_points, const std::size_t num_queries, const std::size_t num_neighbors) const;

    KNNQueue* nearestNeighborsSearch(const float* query_point, const std::size_t num_neighbors);

};
