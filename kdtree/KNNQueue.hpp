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
template <typename ItemT, typename DistanceT>
inline DistanceT distanceBetween(const ItemT* p1, const ItemT* p2, const std::size_t size) {
    DistanceT distance = 0.0;

    for (std::size_t i = 0; i < size; ++i) {
        DistanceT diff = p2[i] - p1[i];
        distance += (diff * diff);
    }

    return distance;
}


template <typename ItemT, typename DistanceT>
class KNNQueue {
private:
    const ItemT* query_point;
    ItemT* potential_neighbor;
    std::size_t num_neighbors;
    std::size_t num_dimensions;
    std::size_t current_size = 0;

    friend class ThreadSafeKNNQueue;

    inline bool closerThanFarthestNeighbor(const DistanceT p) const {
        return p < this->top().distance_from_queried_point;
    }

public:
    Neighbor<DistanceT>* array;

    KNNQueue() = delete;

    KNNQueue(const float* query_point_in, const std::size_t num_neighbors_in, const std::size_t num_dimensions_in):
        query_point(query_point_in),
        num_neighbors(num_neighbors_in),
        num_dimensions(num_dimensions_in)
    {
        this->array = new Neighbor<DistanceT>[num_neighbors_in];
        this->potential_neighbor = new float[num_dimensions_in];
    }

    ~KNNQueue() {
//        delete[] this->array;
        delete[] this->potential_neighbor;
    }

    inline bool empty() const {
        return this->current_size == 0;
    }

    inline Neighbor<DistanceT>& top() const {
        return this->array[0];
    }

    inline bool full() const {
        return this->current_size == this->num_neighbors;
    }

    inline float* getPotentialNeighbor() const {
        return this->potential_neighbor;
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
//        if (!this->full()) {
//            this->heapify();
//        }
        std::sort(this->array, this->array + this->num_neighbors);
    }

    void siftDownRoot();

    void registerAsNeighbor(std::size_t index);

    void registerAsNeighborIfCloser(std::size_t index);
};


/*
 * Standard binary heap siftdown function. Sifts the root index down until it is in a valid position.
 */
    template <typename ItemT, typename DistanceT>
    void KNNQueue<ItemT, DistanceT>::siftDownRoot() {
        std::size_t index = 0;
        std::size_t first_index_without_both_children = this->current_size / 2 - (this->current_size % 2 == 0);

        while (index < first_index_without_both_children) {
            std::size_t left_child = 2 * index + 1;
            std::size_t larger_child = left_child + (this->array[left_child] < this->array[left_child + 1]);
            std::size_t swap_destination = index + ((larger_child - index) & (0 - (this->array[index] < this->array[larger_child])));

            if (index == swap_destination) { return; }

            std::swap(
                    this->array[index],
                    this->array[swap_destination]
            );

            index = swap_destination;
        }

        // Special case for when the index only has one child.
        if (index == first_index_without_both_children && (this->current_size % 2 == 0)) {
            std::size_t left_child = 2 * index + 1;
            std::size_t swap_destination = index + ((left_child - index) & (0 - (this->array[index] < this->array[left_child])));

            std::swap(
                    this->array[index],
                    this->array[swap_destination]
            );
        }
    }

/*
 * Adds the potential neighbor to the queue, regardless of its distance from the query point.
 * Does not keep the array heapified, but does keep the farthest element from the query point at i=0.
 * Assumes that the array is not full.
 */
    template <typename ItemT, typename DistanceT>
    void KNNQueue<ItemT, DistanceT>::registerAsNeighbor(std::size_t index) {
        double distance_from_query = distanceBetween(this->query_point, this->potential_neighbor, this->num_dimensions);

        this->array[current_size].index = index;
        this->array[this->current_size].distance_from_queried_point = distance_from_query;

        // Swap the new neighbor with the neighbor at i=0 if its distance from the query point is greater.
        std::swap(
                this->array[0],
                this->array[this->current_size & (0 - (this->array[0].distance_from_queried_point < this->array[current_size].distance_from_queried_point))]
        );

        ++this->current_size;
    }

    template <typename ItemT, typename DistanceT>
    void KNNQueue<ItemT, DistanceT>::registerAsNeighborIfCloser(std::size_t index) {
        // If the priority queue is at capacity and the potential neighbor is closer to the query point than the current
        // furthest neighbor, remove the furthest neighbor from the priority queue and push the potential neighbor.
        double distance_from_query = distanceBetween(this->query_point, this->potential_neighbor, this->num_dimensions);

        if (this->closerThanFarthestNeighbor(distance_from_query)) {
            this->array[0].index = index;
            this->array[0].distance_from_queried_point = distance_from_query;

            this->siftDownRoot();
        }
    }

}