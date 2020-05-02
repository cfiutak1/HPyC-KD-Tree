#ifndef KDTREE_HPP
#define KDTREE_HPP

#include "../filedata/TrainingFileData.hpp"
#include "../quickselect/AdaptiveQuickselect.hpp"
//#include "Point.hpp"
#include "KDNode.hpp"

#include <cstdint>
#include <vector>
#include <map>
#include <future>


class KDTree {
private:
    KDNode* root = nullptr;
    unsigned int num_threads = 0;
    uint64_t num_points = 0;
    uint64_t num_dimensions = 0;
    std::vector<KDNode*>& points;
    std::vector<AdaptiveQuickselect*> selectors;

    unsigned int height = 0;

public:
    KDTree(
        unsigned int num_threads_in,
        TrainingFileData* training_file_data_in,
        std::vector<KDNode*>& points_in
    ):
        num_threads(num_threads_in),
        num_points(training_file_data_in->num_points),
        num_dimensions(training_file_data_in->num_dimensions),
        points(points_in),
        height(std::ceil(log2(num_points)))
    {
        this->selectors.resize(num_dimensions);
        for (uint64_t i = 0; i < this->num_dimensions; ++i) {
            AdaptiveQuickselect* q = new AdaptiveQuickselect(i);
            selectors.push_back(q);
        }
    }

    ~KDTree() {
        this->destructorHelper(this->root);
        for (auto q : this->selectors) delete q;
    }

    void destructorHelper(KDNode* n) {
        if (n == nullptr) return;

        this->destructorHelper(n->left_child);
        this->destructorHelper(n->right_child);

        delete n;
    }

    KDNode* getRoot() { return this->root; }
//    uint64_t getNumPoints() { return this->num_points; }
    uint64_t getNumDimensions() { return this->num_dimensions; }
//    unsigned int getHeight() { return this->height; }


    void buildTreeParallel();
    KDNode* buildSubTreeDepthFirst(const uint64_t& begin, const uint64_t& end, uint64_t depth, long allocated_threads);
};

#endif
