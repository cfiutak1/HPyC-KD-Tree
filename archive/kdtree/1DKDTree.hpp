#pragma once

#include "KNNQueue.hpp"
#include "../filedata/TrainingFileData.hpp"

#include <cstdint>


class KDTree1D {
private:
    alignas(64) float* nodes;
    uint64_t num_dimensions;
    uint64_t num_points;

    void buildTree(const uint64_t subarray_begin, const uint64_t subarray_end, unsigned int depth);

    void nearestNeighborsSearch(const float* query_point, uint64_t begin, uint64_t end, uint64_t depth, KNNQueue& nearest_neighbors) const;

    inline float& cellAt(const uint64_t node_index, const unsigned int depth) const;
    inline float* nodeAt(const uint64_t node_index) const;

public:
    KDTree1D(float* nodes_in, const uint64_t& num_points_in, uint64_t num_dimensions_in):
        nodes(nodes_in),
        num_dimensions(num_dimensions_in),
        num_points(num_points_in)
    {
        this->buildTree(0, this->num_points, 0);
    }

    KNNQueue nearestNeighborsSearch(const float* query_point, const uint64_t& num_neighbors) const;
};
