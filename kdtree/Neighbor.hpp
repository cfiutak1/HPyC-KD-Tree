#ifndef NEIGHBOR_HPP
#define NEIGHBOR_HPP


class Neighbor {
public:
    const float* point;
    double distance_from_queried_point;

    Neighbor() = delete;

    Neighbor(const float* point_in, double distance_from_queried_point_in):
        point(point_in),
        distance_from_queried_point(distance_from_queried_point_in)
    {}
};


class NeighborComparator {
public:
    NeighborComparator() = default;

    bool operator()(const Neighbor& n1, const Neighbor& n2) {
        return n1.distance_from_queried_point < n2.distance_from_queried_point;
    }
};

#endif
