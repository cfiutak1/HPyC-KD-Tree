#ifndef ARRAYKDTREE_HPP
#define ARRAYKDTREE_HPP

#include "KNNQueue.hpp"
#include "../filedata/TrainingFileData.hpp"

#include <cstdint>


class KDTree {
private:
    alignas(32) float** nodes;
    uint64_t num_dimensions;
    uint64_t num_points;

    void buildTree(const uint64_t subarray_begin, const uint64_t subarray_end, unsigned int depth);

    void nearestNeighborsSearch(const float* query_point, uint64_t begin, uint64_t end, uint64_t depth, KNNQueue& nearest_neighbors) const;

    inline float* pointAt(const std::size_t index) const;
    inline void swap(std::size_t index1, std::size_t index2);

public:
    KDTree(float** nodes_in, const uint64_t& num_points_in, uint64_t num_dimensions_in):
        nodes(nodes_in),
        num_dimensions(num_dimensions_in),
        num_points(num_points_in)
    {
        this->buildTree(0, this->num_points, 0);
    }

    KNNQueue nearestNeighborsSearch(const float* query_point, const uint64_t& num_neighbors) const;
};


#endif