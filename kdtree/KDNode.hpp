#ifndef KDNODE_HPP
#define KDNODE_HPP

//#include "KDNode.hpp"
#include <cmath>
#include <cstdint>

class KDNode {
public:
    const uint64_t& num_dimensions;
    float* coordinates;

    KDNode* left_child = nullptr;
    KDNode* right_child = nullptr;
    uint64_t depth = 0;


    KDNode(const uint64_t& num_dimensions_in): num_dimensions(num_dimensions_in) {
        this->coordinates = new float[num_dimensions_in];
    }

    ~KDNode() {
        delete this->coordinates;
    }

    inline float getCoordinate(uint64_t dimension) const {
        return this->coordinates[dimension % this->num_dimensions];
    }

    inline float operator[](const uint64_t dimension) const {
        return this->coordinates[dimension % this->num_dimensions];
    }

    float distanceBetween(KDNode* n) const {
        float distance = 0.0;

        for (uint64_t i = 0; i < this->num_dimensions; i++) {
            float diff_between_dimensions = this->coordinates[i] - n->getCoordinate(i);
            distance += pow(diff_between_dimensions, 2);
        }

        return distance;
    }
};

inline bool comp_lt(const KDNode* n1, const KDNode* n2, const unsigned long& dimension) {
    return n1->getCoordinate(dimension) < n2->getCoordinate(dimension);
}

inline bool comp_gt(const KDNode* n1, const KDNode* n2, const unsigned long& dimension) {
    return n1->getCoordinate(dimension) > n2->getCoordinate(dimension);
}

inline bool comp_lte(const KDNode* n1, const KDNode* n2, const unsigned long& dimension) {
    return n1->getCoordinate(dimension) <= n2->getCoordinate(dimension);
}

inline bool comp_gte(const KDNode* n1, const KDNode* n2, const unsigned long& dimension) {
    return n1->getCoordinate(dimension) >= n2->getCoordinate(dimension);
}

#endif
