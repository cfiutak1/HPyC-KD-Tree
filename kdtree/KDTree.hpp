#ifndef ARRAYKDTREE_HPP
#define ARRAYKDTREE_HPP

#include "KNNQueue.hpp"
#include "../filedata/TrainingFileData.hpp"

#include <cstdint>
#include <atomic>


class KDTree {
private:
    friend class ParallelKDTree;

    float** nodes;
    std::size_t num_dimensions;
    std::size_t num_points;



    void buildTree(const std::size_t subarray_begin, const std::size_t subarray_end, unsigned int depth);

    void nearestNeighborsSearch(const float* query_point, std::size_t begin, std::size_t end, std::size_t depth, KNNQueue& nearest_neighbors) const;

    inline void readPointAt(float* point, const std::size_t index) const {
        for (std::size_t i = 0; i < this->num_dimensions; ++i) {
            point[i] = this->nodes[i][index];
        }
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

    KDTree(float** nodes_in, const std::size_t num_points_in, std::size_t num_dimensions_in):
        nodes(nodes_in),
        num_dimensions(num_dimensions_in),
        num_points(num_points_in)
    {
        this->buildTree(0, this->num_points, 0);
    }

    void nearestNeighborsSearch(const float* query_point, KNNQueue& queue) const;
};


#endif