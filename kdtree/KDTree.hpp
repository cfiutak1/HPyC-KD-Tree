#ifndef ARRAYKDTREE_HPP
#define ARRAYKDTREE_HPP


#include "../filedata/TrainingFileData.hpp"
#include "../quickselect/AdaptiveQuickselect.hpp"

//#include "KDNode.hpp"
#include "KNNQueue.hpp"

#include <cstdint>
#include <vector>
#include <cmath>


class KDTree {
private:
    std::vector<float*> nodes;
    uint64_t num_dimensions;
    uint64_t num_points;
    uint64_t height;
    std::vector<AdaptiveQuickselect*> selectors;

public:
    KDTree(std::vector<float*>& nodes_in, uint64_t num_dimensions_in):
        nodes(nodes_in),
        num_dimensions(num_dimensions_in),
        num_points(nodes_in.size()),
        height(log2(num_points))
    {
        this->selectors.reserve(num_dimensions_in);

        for (uint64_t i = 0; i < this->num_dimensions; ++i) {
            AdaptiveQuickselect* q = new AdaptiveQuickselect(i);
            selectors.push_back(q);
        }
    }

    ~KDTree() {
        for (auto n : this->nodes) { delete n; }
        for (auto s : this->selectors) { delete s; }
    }

    void buildTree();
    void buildSubTree(const uint64_t& begin, const uint64_t& end, uint64_t depth);

    KNNQueue* nearestNeighborsSearch(float* query_point, uint64_t num_neighbors);
    void nearestNeighborsSearchHelper(float* query_point, uint64_t begin, uint64_t end, uint64_t depth, KNNQueue& queue);
};


#endif