#include <cstdio>

#pragma once


class alignas(16) Neighbor {
public:
    float* point;
    double distance_from_queried_point;

    Neighbor() {
//        printf("%s:%d Neighbor default constructor called\n", __FILE__, __LINE__);
    }

    Neighbor(float* point_in, double distance_from_queried_point_in):
            point(point_in),
            distance_from_queried_point(distance_from_queried_point_in)
    {
//        printf("%s:%d Neighbor proper constructor called\n", __FILE__, __LINE__);
    }




    ~Neighbor() {
//        printf("%s:%d Neighbor destructor called\n", __FILE__, __LINE__);
    }

    bool operator < (const Neighbor& n) const {
        return n.distance_from_queried_point < this->distance_from_queried_point;
    }
};
