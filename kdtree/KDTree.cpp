#include "KDTree.hpp"

#include "../quickselect/AdaptiveQuickselect.hpp"

#include <algorithm>
#include <iterator>
#include <future>


void KDTree::buildTreeParallel() {
    uint64_t begin = 0;
    uint64_t end = this->points.size();
    this->root = this->buildSubTreeDepthFirst(begin, end, 0, this->num_threads);
}


KDNode* KDTree::buildSubTreeDepthFirst(const uint64_t& begin, const uint64_t& end, uint64_t depth, long allocated_threads) {
    uint64_t range = end - begin;

    if (range == 1) {
        this->points[begin]->depth = depth;

        return this->points[begin];
    }

    else if (range == 2) {
        KDNode* parent;
        KDNode* child;

        if (comp_lt(this->points[begin], this->points[begin + 1], depth - 1)) {
            parent = this->points[begin + 1];
            child = this->points[begin];
        }

        else {
            parent = this->points[begin];
            child = this->points[begin + 1];
        }

        if (comp_lt(child, parent, depth)) { parent->left_child = child; }
        else { parent->right_child = child; }

        parent->depth = depth;
        child->depth = depth + 1;

        return parent;
    }

    std::vector<KDNode*>::iterator front = this->points.begin() + begin;

    uint64_t median = range / 2;

    this->selectors[depth % this->num_dimensions]->adaptiveQuickselect(this->points.begin() + begin, this->points.begin() + end, median);

    KDNode* value = *(front + median);

    uint64_t l_begin = begin;
    uint64_t l_end = l_begin + median;

    uint64_t r_begin = begin + median + 1;
    uint64_t r_end = end;


    if (allocated_threads > 0) {
        long l_threads = allocated_threads / 2;
        long r_threads = allocated_threads - l_threads - 1;

        std::future<KDNode*> right_child_future = std::async(
            &KDTree::buildSubTreeDepthFirst,
            this,
            r_begin, r_end, depth + 1, r_threads
        );

        KDNode* left_child = this->buildSubTreeDepthFirst(l_begin, l_end, depth + 1, l_threads);

        value->left_child = left_child;
        value->right_child = right_child_future.get();
        value->depth = depth;

        return value;
    }

    KDNode* left_child = this->buildSubTreeDepthFirst(l_begin, l_end, depth + 1, 0);
    KDNode* right_child = this->buildSubTreeDepthFirst(r_begin, r_end, depth + 1, 0);

    value->left_child = left_child;
    value->right_child = right_child;
    value->depth = depth;

    return value;
}
