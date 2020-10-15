#include <cstdint>

namespace hpyc {

template <typename DistanceT>
class Neighbor {
public:
    std::size_t index;
    DistanceT distance_from_queried_point;

    Neighbor() = default;

    Neighbor(std::size_t index_in, DistanceT distance_from_queried_point_in):
        index(index_in),
        distance_from_queried_point(distance_from_queried_point_in)
    {}

    bool operator < (const Neighbor<DistanceT>& n) const {
        return this->distance_from_queried_point < n.distance_from_queried_point;
    }
};

}