#ifndef ARRAYKDTREE_HPP
#define ARRAYKDTREE_HPP

#include "KNNQueue.hpp"
#include "../filedata/TrainingFileData.hpp"

#include <cstdint>
#include <atomic>


class KDTree {
private:
    friend class ParallelKDTree;

    alignas(32) float** nodes;
    uint64_t num_dimensions;
    uint64_t num_points;

    std::atomic<int> available_threads;
//    KNNQueue results;

    void buildTree(const uint64_t subarray_begin, const uint64_t subarray_end, unsigned int depth);

    void nearestNeighborsSearch(const float* query_point, uint64_t begin, uint64_t end, uint64_t depth, KNNQueue& nearest_neighbors) const;

    inline void readPointAt(const std::size_t index, float* point) const {
//        alignas(32) float* point = new float[this->num_dimensions];

        for (uint64_t i = 0; i < this->num_dimensions; ++i) {
            point[i] = this->nodes[i][index];
        }

//        return point;
    }

    inline void swap(std::size_t index1, std::size_t index2) {
        for (std::size_t i = 0; i < this->num_dimensions; ++i) {
            std::swap(
                    this->nodes[i][index1],
                    this->nodes[i][index2]
            );
        }
    }

public:
    KDTree() = default;

    KDTree(float** nodes_in, const uint64_t num_points_in, uint64_t num_dimensions_in):
        nodes(nodes_in),
        num_dimensions(num_dimensions_in),
        num_points(num_points_in)
    {
        this->buildTree(0, this->num_points, 0);
    }

    void nearestNeighborsSearch(const float* query_point, KNNQueue& queue) const;
};


#endif