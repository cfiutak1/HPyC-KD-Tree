#include "Neighbor.hpp"
#include "../memory_pool/NeighborPointRecycler.hpp"

#include <queue>
#include <vector>
#include <cstdint>
#include <algorithm>

#pragma once

namespace hpyc {

/*
 * Free function that computes the euclidean squared distance between two float arrays of equal size.
 */
inline double distanceBetween(const float* p1, const float* p2, const std::size_t size) {
    double distance = 0.0;

    for (std::size_t i = 0; i < size; ++i) {
        double diff = p2[i] - p1[i];
        distance += (diff * diff);
    }

    return distance;
}


class KNNQueue {
private:
    const float* query_point;
    std::size_t num_neighbors;
    std::size_t num_dimensions;
    std::size_t current_size = 0;
    NeighborPointRecycler& point_allocator;

    friend class ThreadSafeKNNQueue;

    inline bool closerThanFarthestNeighbor(const double p) const {
        return p < this->top().distance_from_queried_point;
    }

public:
    Neighbor* array;

    KNNQueue() = delete;

    KNNQueue(const float* query_point_in, const std::size_t num_neighbors_in, const std::size_t num_dimensions_in, NeighborPointRecycler& point_allocator_in):
        query_point(query_point_in),
        num_neighbors(num_neighbors_in),
        num_dimensions(num_dimensions_in),
        point_allocator(point_allocator_in)
    {
        this->array = new Neighbor[num_neighbors_in];

        for (std::size_t i = 0; i < this->num_neighbors; ++i) {
            this->array[i] = Neighbor(this->point_allocator.getPoint());
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

    inline double distanceFromPotentialNeighbor() {
        return distanceBetween(this->query_point, getPotentialNeighbor(), this->num_dimensions);
    }

    inline void heapify() {
        std::make_heap(this->array, this->array + this->num_neighbors);
    }

    /*
     * Heapifies the array if the array hasn't been heapified yet and returns the memory to the point recycler.
     */
    inline void validate() {
        if (!this->full()) {
            this->heapify();
        }

        this->point_allocator.resetCount();
    }

    void siftDownRoot();

    void registerAsNeighbor();

    void registerAsNeighborIfCloser();
};

}