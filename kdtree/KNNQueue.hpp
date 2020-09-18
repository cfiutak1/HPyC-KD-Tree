#include "Neighbor.hpp"

#include <queue>
#include <vector>
#include <cstdint>

#pragma once


class KNNQueue {
private:
    const float* query_point;
    const uint64_t& num_neighbors;
    const uint64_t& num_dimensions;

    std::priority_queue<Neighbor, std::vector<Neighbor>, NeighborComparator> nearest_neighbors;

    bool closerThanFarthestNeighbor(const double& p) const;

public:
    KNNQueue() = delete;

    KNNQueue(const float* query_point_in, const uint64_t& num_neighbors_in, const uint64_t& num_dimensions_in):
        query_point(query_point_in),
        num_neighbors(num_neighbors_in),
        num_dimensions(num_dimensions_in),
        nearest_neighbors(NeighborComparator{})
    {}

    ~KNNQueue() {}

    inline bool empty() const { return this->nearest_neighbors.empty(); }

    inline void pop() {
        this->nearest_neighbors.pop();
    }

    inline void replaceFarthestNeighbor() {
        delete[] this->nearest_neighbors.top().point;
        this->nearest_neighbors.pop();
    }

    inline Neighbor top() const { return this->nearest_neighbors.top(); }

    inline bool isFull() const { return this->nearest_neighbors.size() == this->num_neighbors; }

    bool registerAsNeighborIfCloser(float* potential_neighbor);
};
