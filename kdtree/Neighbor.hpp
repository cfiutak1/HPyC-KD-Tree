#pragma once


class alignas(16) Neighbor {
public:
    float* point;
    double distance_from_queried_point;

    Neighbor() {}

    Neighbor(float* point_in, double distance_from_queried_point_in):
            point(point_in),
            distance_from_queried_point(distance_from_queried_point_in)
    {}

    bool operator < (const Neighbor& n) const {
        return this->distance_from_queried_point < n.distance_from_queried_point;
    }
};
