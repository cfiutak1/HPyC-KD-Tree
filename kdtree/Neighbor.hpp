#include <cstdint>

namespace hpyc {

class Neighbor {
public:
//    float* point;
    std::size_t index;
    double distance_from_queried_point;

    Neighbor() = default;

    Neighbor(std::size_t index_in, double distance_from_queried_point_in):
        index(index_in),
        distance_from_queried_point(distance_from_queried_point_in)
    {}

    bool operator < (const Neighbor& n) const {
        return this->distance_from_queried_point < n.distance_from_queried_point;
    }

//    bool operator > (const Neighbor& n) const {
//        return this->distance_from_queried_point > n.distance_from_queried_point;
//    }
};

}