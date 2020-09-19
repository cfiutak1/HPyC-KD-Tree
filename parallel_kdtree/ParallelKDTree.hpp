#include "ThreadSafeKNNQueue.hpp"
#include "../kdtree/KDTree.hpp"

#include <cstdint>
#include <atomic>

#pragma once


class ParallelKDTree {
private:
    KDTree* tree;
    std::atomic<int> available_threads{};
    ThreadSafeKNNQueue nearest_neighbors;

    void buildTree(const uint64_t subarray_begin, const uint64_t subarray_end, unsigned int depth, unsigned int num_threads);

    void nearestNeighborsSearchTS(const float* query_point, uint64_t begin, uint64_t end, const uint64_t depth, const uint64_t num_neighborsd);


public:
    // TODO It might be worth having fields for these instead in the ParallelKDTree class as well
    ParallelKDTree(float** nodes_in, const uint64_t num_points_in, uint64_t num_dimensions_in, int num_threads_in) {
        this->tree = new KDTree();
        this->tree->nodes = nodes_in;
        this->tree->num_points = num_points_in;
        this->tree->num_dimensions = num_dimensions_in;
        this->available_threads = num_threads_in;

        this->buildTree(0, num_points_in, 0, num_threads_in);
    }

    ~ParallelKDTree() {
        delete this->tree;
    }

    KNNQueue* nearestNeighborsSearches(float** query_points, const uint64_t num_queries, const uint64_t num_neighbors, unsigned int num_threads) const;

    ThreadSafeKNNQueue nearestNeighborsSearch(const float* query_point, const uint64_t num_neighbors);

};
