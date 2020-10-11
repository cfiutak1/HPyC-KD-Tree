#include "KNNQueue.hpp"


/*
 * Standard binary heap siftdown function. Sifts the root index down until it is in a valid position.
 */
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
void KNNQueue::registerAsNeighbor() {
    double distance_from_query = this->distanceFromPotentialNeighbor();

    // Swap the memory in the empty neighbor at i=current_size with the memory containing the potential neighbor.
    std::swap_ranges(
        this->array[current_size].point,
        this->array[current_size].point + this->num_dimensions,
        this->getPotentialNeighbor()
    );

    // Add the distance to the neighbor object at i=current_size.
    this->array[this->current_size].distance_from_queried_point = distance_from_query;

    // Swap the new neighbor with the neighbor at i=0 if its distance from the query point is greater.
    std::swap(
        this->array[0],
        this->array[this->current_size & (0 - (this->array[0].distance_from_queried_point < this->array[current_size].distance_from_queried_point))]
    );

    ++this->current_size;
}


void KNNQueue::registerAsNeighborIfCloser() {
    double distance_from_query = distanceBetween(this->query_point, this->getPotentialNeighbor(), this->num_dimensions);

    // If the priority queue is at capacity and the potential neighbor is closer to the query point than the current
    // furthest neighbor, remove the furthest neighbor from the priority queue and push the potential neighbor.
    if (this->closerThanFarthestNeighbor(distance_from_query)) {
        std::swap_ranges(
            this->array[0].point,
            this->array[0].point + this->num_dimensions,
            this->getPotentialNeighbor()
        );

        this->array[0].distance_from_queried_point = distance_from_query;

        this->siftDownRoot();
    }
}
