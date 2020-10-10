#include "Neighbor.hpp"
#include "../memory_pool/NeighborPointRecycler.hpp"

#include <queue>
#include <vector>
#include <cstdint>
#include <mutex>
#include <algorithm>

#pragma once

/*
 * Free function that computes the euclidean squared distance between two float arrays of equal size.
 */
inline double distanceBetween(const float* p1, const float* p2, const int& size) {
    double distance = 0.0;

    for (int i = 0; i < size; ++i) {
        double diff = p2[i] - p1[i];
        distance += (diff * diff);
    }

    return distance;
}


class KNNQueue {
private:
    float* query_point;
    uint64_t num_neighbors;
    uint64_t num_dimensions;
    std::size_t current_size = 0;
    NeighborPointRecycler& point_allocator;

    friend class ThreadSafeKNNQueue;

    inline bool closerThanFarthestNeighbor(const double& p) const {
        return p < this->top().distance_from_queried_point;
    }

public:
    Neighbor* array;

    KNNQueue() = delete;

    KNNQueue(float* query_point_in, const uint64_t& num_neighbors_in, const uint64_t& num_dimensions_in, NeighborPointRecycler& point_allocator_in):
        query_point(query_point_in),
        num_neighbors(num_neighbors_in),
        num_dimensions(num_dimensions_in),
        point_allocator(point_allocator_in)
    {
        this->array = new Neighbor[num_neighbors_in];

        // TODO look into placement new
        for (std::size_t i = 0; i < this->num_neighbors; ++i) {
            this->array[i] = Neighbor(this->point_allocator.getPoint(), this->num_dimensions);
        }
    }

    ~KNNQueue() {
        delete[] this->array;
    }

    inline bool empty() const {
        return this->current_size == 0;
    }

    inline Neighbor& top() const {
        return this->array[0];
    }

    inline bool full() const {
        return this->current_size == this->num_neighbors;
    }

    inline float* getPotentialNeighbor() const {
        return this->point_allocator.potential_neighbor;
    }

    inline void validate() {
        if (!this->full()) {
            std::make_heap(this->array, this->array + this->current_size);
        }

        this->point_allocator.resetCount();
    }

    bool registerAsNeighborIfNotFull(double distance_from_query);

    void siftDownRoot();

    inline bool registerAsNeighborIfCloser(double distance_from_potential_query);

    bool registerAsNeighborIfEligible();
};
