#ifndef ARRAYKDTREE_HPP
#define ARRAYKDTREE_HPP

#include "KNNQueue.hpp"
#include "../filedata/TrainingFileData.hpp"
#include "../quickselect/AdaptiveQuickselect.hpp"

#include <cstdint>
#include <vector>
#include <cmath>


class KDTree {
private:
    std::vector<float*> nodes;
    uint64_t num_dimensions;
    uint64_t num_points;

    std::vector<AdaptiveQuickselect> selectors;

    void buildTree(const uint64_t subarray_begin, const uint64_t subarray_end, unsigned int depth);

    void nearestNeighborsSearch(const float* query_point, uint64_t begin, uint64_t end, uint64_t depth, KNNQueue& nearest_neighbors) const;

public:
    KDTree(std::vector<float*>& nodes_in, uint64_t num_dimensions_in):
        nodes(nodes_in),
        num_dimensions(num_dimensions_in),
        num_points(nodes_in.size())
    {
        this->selectors.reserve(num_dimensions_in);

        for (uint64_t i = 0; i < this->num_dimensions; ++i) {
            AdaptiveQuickselect q(i);
            selectors.push_back(q);
        }

        this->buildTree(0, this->num_points, 0);
    }

    KNNQueue nearestNeighborsSearch(const float* query_point, const uint64_t& num_neighbors) const;
};


#endif