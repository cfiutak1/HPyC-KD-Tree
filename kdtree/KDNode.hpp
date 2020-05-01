#ifndef KDNODE_HPP
#define KDNODE_HPP

#include "Point.hpp"


class KDNode {
public:
    Point* point;
    KDNode* left_child = nullptr;
    KDNode* right_child = nullptr;
    uint64_t depth = 0;

    // KDNode(Point* point_in): point(point_in) {}

    KDNode(Point* point_in, uint64_t depth_in): point(point_in), depth(depth_in) {}

    KDNode(Point* point_in, KDNode* left_child_in, KDNode* right_child_in, uint64_t depth_in):
        point(point_in),
        left_child(left_child_in),
        right_child(right_child_in),
        depth(depth_in)
    {}

    inline bool isLeaf() { return this->left_child == nullptr && this->right_child == nullptr; }
};

#endif
