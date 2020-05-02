#ifndef NEIGHBOR_HPP
#define NEIGHBOR_HPP

//#include "Point.hpp"
#include "KDNode.hpp"

class Neighbor {
public:
    KDNode* point;
    double distance_from_queried_point;

    Neighbor(KDNode* point, double distance_from_queried_point) {
        this->point = point;
        this->distance_from_queried_point = distance_from_queried_point;
    }
};


class NeighborComparator {
public:
    bool operator()(const Neighbor& n1, const Neighbor& n2) {
        return n1.distance_from_queried_point < n2.distance_from_queried_point;
    }
};

#endif
