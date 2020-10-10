#include "KNNQueue.hpp"


inline bool KNNQueue::registerAsNeighborIfNotFull(double distance_from_query) {
    // If the priority queue is below capacity, add the potential neighbor regardless of its distance to the query point.
    if (!this->full()) {
        std::swap_ranges(
            this->array[current_size].point,
            this->array[current_size].point + this->num_dimensions,
            this->getPotentialNeighbor()
        );

        this->array[this->current_size].distance_from_queried_point = distance_from_query;

        std::swap(
            this->array[0],
            this->array[this->current_size & (0 - (this->array[0].distance_from_queried_point < this->array[current_size].distance_from_queried_point))]
        );

        ++this->current_size;

        if (this->current_size == this->num_neighbors) {
            std::make_heap(this->array, this->array + this->num_neighbors);
        }

        return true;
    }

    return false;
}


void KNNQueue::siftDownRoot() {
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

    if (index == first_index_without_both_children && (this->current_size % 2 == 0)) {
        std::size_t left_child = 2 * index + 1;
        std::size_t swap_destination = index + ((left_child - index) & (0 - (this->array[index] < this->array[left_child])));

        std::swap(
            this->array[index],
            this->array[swap_destination]
        );
    }
}


inline bool KNNQueue::registerAsNeighborIfCloser(double distance_from_potential_query) {
    // If the priority queue is at capacity and the potential neighbor is closer to the query point than the current
    // furthest neighbor, remove the furthest neighbor from the priority queue and push the potential neighbor.
    if (this->closerThanFarthestNeighbor(distance_from_potential_query)) {
        std::swap_ranges(
            this->array[0].point,
            this->array[0].point + this->num_dimensions,
            this->getPotentialNeighbor()
        );

        this->array[0].distance_from_queried_point = distance_from_potential_query;

        this->siftDownRoot();

        return true;
    }

    return false;
}


/*
 * Public member function that adds a point to the priority queue of nearest neighbors if the priority queue is below
 * capacity or if the point is closer than an existing neighbor. If the point is closer than an existing neighbor AND
 * the priority queue is at capacity, removes the furthest neighbor before adding the point.
 */
bool KNNQueue::registerAsNeighborIfEligible() {
    double distance_from_query = distanceBetween(this->query_point, this->getPotentialNeighbor(), this->num_dimensions);

    if (this->registerAsNeighborIfNotFull(distance_from_query)) { return true; }

    if (this->registerAsNeighborIfCloser(distance_from_query)) { return true; }

    return false;
}
