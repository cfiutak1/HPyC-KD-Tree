#ifndef KNNQUEUE_HPP
#define KNNQUEUE_HPP

#include "Neighbor.hpp"

#include <queue>
#include <vector>
#include <cstdint>


class KNNQueue {
private:
    const float* query_point;
    const uint64_t& num_neighbors;
    const uint64_t& num_dimensions;
    // TODO make this const static again
    const static NeighborComparator comp;
    std::priority_queue<Neighbor, std::vector<Neighbor>, NeighborComparator> nearest_neighbors;

    bool closerThanFarthestNeighbor(const double& p) const;

public:
    KNNQueue() = delete;

    KNNQueue(const float* query_point_in, const uint64_t& num_neighbors_in, const uint64_t& num_dimensions_in):
        query_point(query_point_in),
        num_neighbors(num_neighbors_in),
        num_dimensions(num_dimensions_in),
        nearest_neighbors(comp)
    {
//        this->comp = NeighborComparator();
//        this->nearest_neighbors = std::priority_queue<Neighbor, std::vector<Neighbor>, NeighborComparator>(comp);
    }

    ~KNNQueue() {}

    inline bool empty() const { return this->nearest_neighbors.empty(); }

    inline void pop() { this->nearest_neighbors.pop(); }

    inline Neighbor top() const { return this->nearest_neighbors.top(); }

    inline bool isFull() const { return this->nearest_neighbors.size() == this->num_neighbors; }

    bool registerAsNeighborIfCloser(float* potential_neighbor);
};

#endif
