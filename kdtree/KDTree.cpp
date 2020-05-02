#include "KDTree.hpp"

#include "../quickselect/AdaptiveQuickselect.hpp"

#include <cstring>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include <cassert>
#include <thread>


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
//        return new KDNode(*(this->points.begin() + begin), depth);
    }

    else if (range == 2) {
        KDNode* child = this->points[begin];
        child->depth = depth + 1;
//        KDNode* new_node = new KDNode(*(this->points.begin() + begin), depth + 1);

        if (this->points.at(begin)->getCoordinate(depth + 1) < this->points.at(begin + 1)->getCoordinate(depth + 1)) {
            KDNode* new_node = this->points[begin + 1];
            new_node->left_child = child;
            new_node->depth = depth;
            return new_node;
//            return new KDNode(
//                *(this->points.begin() + begin + 1),
//                child,
//                nullptr,
//                depth
//            );
        }

        KDNode* new_node = this->points[begin + 1];
        new_node->right_child = child;
        new_node->depth = depth;

        return new_node;

//        return new KDNode(
//            *(this->points.begin() + begin + 1),
//            nullptr,
//            new_node,
//            depth
//        );

    }

    std::vector<KDNode*>::iterator front = this->points.begin() + begin;

    uint64_t median = range / 2;
    AdaptiveQuickselect selector(depth);
    selector.adaptiveQuickselect(this->points.begin() + begin, this->points.begin() + end, median);

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

//        return new KDNode(value, left_child, right_child_future.get(), depth);
    }

    KDNode* left_child = this->buildSubTreeDepthFirst(l_begin, l_end, depth + 1, 0);
    KDNode* right_child = this->buildSubTreeDepthFirst(r_begin, r_end, depth + 1, 0);

    value->left_child = left_child;
    value->right_child = right_child;
    value->depth = depth;

    return value;

//    return new KDNode(value, left_child, right_child, depth);

}
