#include <queue>
#include <vector>
#include <cstdint>
#include <mutex>
#include <algorithm>

#pragma once


class PointAllocator {
private:
    alignas(32) float* allocated_points;
    std::size_t num_dimensions;
    std::size_t num_neighbors;
    std::size_t num_allocated = 0;

public:
    float* potential_neighbor;

    PointAllocator() = delete;

    PointAllocator(std::size_t num_dimensions_in, std::size_t num_neighbors_in):
        num_dimensions(num_dimensions_in),
        num_neighbors(num_neighbors_in)
    {
//        printf("%s:%d\n", __FILE__, __LINE__);
        this->allocated_points = new float[num_dimensions_in * (num_neighbors_in + 1)];
        this->potential_neighbor = this->allocated_points + (num_dimensions_in * num_neighbors_in);
    }

    float* getPoint() {
        float* point = this->allocated_points + (num_allocated * num_dimensions);
        ++this->num_allocated;

        return point;
    }

    void setPotentialNeighbor(float* potential_neighbor_in) {

    }

    void resetCount() {
        this->num_allocated = 0;
    }

    ~PointAllocator() {
        delete[] this->allocated_points;
    }
};


class Neighbor {
public:
    float* point;
    double distance_from_queried_point;
    std::size_t num_dimensions;

    Neighbor() = default;

    Neighbor (float* point_in, std::size_t num_dimensions_in):
        point(point_in),
        num_dimensions(num_dimensions_in)
    {}

    Neighbor(float* point_in, double distance_from_queried_point_in):
        point(point_in),
        distance_from_queried_point(distance_from_queried_point_in)
    {}

    bool operator < (const Neighbor& n) const {
        return this->distance_from_queried_point < n.distance_from_queried_point;
    }

    bool operator > (const Neighbor& n) const {
        return this->distance_from_queried_point > n.distance_from_queried_point;
    }
};


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
    PointAllocator& point_allocator;

    friend class ThreadSafeKNNQueue;

    inline bool closerThanFarthestNeighbor(const double& p) const {
        return p < this->top().distance_from_queried_point;
    }

public:
    Neighbor* array;
//    alignas(32) float* potential_neighbor;

    KNNQueue() = delete;

    KNNQueue(float* query_point_in, const uint64_t& num_neighbors_in, const uint64_t& num_dimensions_in, PointAllocator& point_allocator_in):
        query_point(query_point_in),
        num_neighbors(num_neighbors_in),
        num_dimensions(num_dimensions_in),
        point_allocator(point_allocator_in)
    {
        this->array = new Neighbor[num_neighbors_in];

//        this->point_allocator.potential_neighbor = query_point_in;

        // TODO look into placement new
        for (std::size_t i = 0; i < this->num_neighbors; ++i) {
            this->array[i] = Neighbor(this->point_allocator.getPoint(), this->num_dimensions);
        }

//        this->potential_neighbor = new float[this->num_dimensions];
    }

    ~KNNQueue() {
        delete[] this->array;
//        delete[] this->potential_neighbor;
    }

    inline bool empty() const {
        return this->current_size == 0;
    }

//    inline void pop() {
//        std::pop_heap(this->array, this->array + this->current_size);
//        --this->current_size;
//    }
//
//    inline void removeFarthestNeighbor() {
//        delete[] this->array[0].point;
//        this->pop();
//    }

    inline Neighbor& top() const {
        return this->array[0];
    }

    inline bool full() const {
        return this->current_size == this->num_neighbors;
    }

    inline void validate() {
        if (!this->full()) {
            std::make_heap(this->array, this->array + this->current_size);
        }

        this->point_allocator.resetCount();
    }

    inline float* getPotentialNeighbor() const {
        return this->point_allocator.potential_neighbor;
    }

    inline bool registerAsNeighborIfNotFull(double distance_from_query) {
        // If the priority queue is below capacity, add the potential neighbor regardless of its distance to the query point.
        if (!this->full()) {
//            printf("%s:%d Inserting %lu-th item with distance %f\n", __FILE__, __LINE__, this->current_size, distance_from_query);
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


    void siftDownRoot() {
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

    inline bool registerAsNeighborIfCloser(double distance_from_potential_query) {
        // If the priority queue is at capacity and the potential neighbor is closer to the query point than the current
        // furthest neighbor, remove the furthest neighbor from the priority queue and push the potential neighbor.
        if (this->closerThanFarthestNeighbor(distance_from_potential_query)) {
            // TODO instead of pop + push, just replace and siftdown

//            printf("%s:%d Replacing top (distance=%f) with new item (distance=%f)\n", __FILE__, __LINE__, this->array->distance_from_queried_point, distance_from_potential_query);
            std::swap_ranges(
                this->array[0].point,
                this->array[0].point + this->num_dimensions,
                this->getPotentialNeighbor()
            );

            this->array[0].distance_from_queried_point = distance_from_potential_query;

            this->siftDownRoot();


//            this->removeFarthestNeighbor();
//            ++this->current_size;
//            std::push_heap(this->array, this->array + this->current_size);

            return true;
        }

        return false;
    }


/*
 * Public member function that adds a point to the priority queue of nearest neighbors if the priority queue is below
 * capacity or if the point is closer than an existing neighbor. If the point is closer than an existing neighbor AND
 * the priority queue is at capacity, removes the furthest neighbor before adding the point.
 */
    bool registerAsNeighborIfEligible() {
        double distance_from_query = distanceBetween(this->query_point, this->getPotentialNeighbor(), this->num_dimensions);

        if (this->registerAsNeighborIfNotFull(distance_from_query)) { return true; }

        if (this->registerAsNeighborIfCloser(distance_from_query)) { return true; }

//        delete[] potential_neighbor;

        return false;
    }
};
