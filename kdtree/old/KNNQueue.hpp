#include <queue>
#include <vector>
#include <cstdint>
#include <mutex>

#pragma once


class alignas(16) Neighbor {
public:
    float* point;
    double distance_from_queried_point;

    Neighbor() {}

    Neighbor(float* point_in, double distance_from_queried_point_in):
        point(point_in),
        distance_from_queried_point(distance_from_queried_point_in)
    {}

    bool operator < (const Neighbor& n) const {
        return this->distance_from_queried_point < n.distance_from_queried_point;
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
    const float* query_point;
    uint64_t num_neighbors;
    uint64_t num_dimensions;
    std::size_t current_size = 0;
    Neighbor* array;
    Neighbor* array_begin;

    friend class ThreadSafeKNNQueue;

    inline bool closerThanFarthestNeighbor(const double& p) const {
        return p < this->top().distance_from_queried_point;
    }

public:
    KNNQueue() {}

    KNNQueue(const float* query_point_in, const uint64_t& num_neighbors_in, const uint64_t& num_dimensions_in):
        query_point(query_point_in),
        num_neighbors(num_neighbors_in),
        num_dimensions(num_dimensions_in)
    {
        this->array = new Neighbor[num_neighbors_in + 1];
        this->array_begin = this->array + 1;
    }

    ~KNNQueue() {
        delete[] this->array;
    }

    inline bool empty() const {
        return this->current_size == 0;
    }

    inline void pop() {
        std::pop_heap(this->array_begin, this->array_begin + this->current_size);
        --this->current_size;
    }

    inline void removeFarthestNeighbor() {
        delete[] this->array_begin[0].point;
        this->pop();
    }

    inline Neighbor top() const {
        return this->array_begin[0];
    }

    inline bool full() const {
        return this->current_size == this->num_neighbors;
    }

    inline void validate() {
        if (!this->full()) {
            std::make_heap(this->array_begin, this->array_begin + this->current_size);
        }
    }

    inline bool registerAsNeighborIfNotFull(float* potential_neighbor, double distance_from_query) {
        // If the priority queue is below capacity, add the potential neighbor regardless of its distance to the query point.
        if (!this->full()) {
            this->array_begin[this->current_size].point = potential_neighbor;
            this->array_begin[this->current_size].distance_from_queried_point = distance_from_query;

            ++this->current_size;

            if (this->current_size == this->num_neighbors) {
                std::make_heap(this->array_begin, this->array_begin + this->num_neighbors);
            }

            return true;
        }

        return false;
    }

    inline bool registerAsNeighborIfCloser(float* potential_neighbor, double distance_from_potential_query) {
        // If the priority queue is at capacity and the potential neighbor is closer to the query point than the current
        // furthest neighbor, remove the furthest neighbor from the priority queue and push the potential neighbor.
        if (this->closerThanFarthestNeighbor(distance_from_potential_query)) {
            this->removeFarthestNeighbor();
            this->array_begin[this->current_size] = {potential_neighbor, distance_from_potential_query};
            ++this->current_size;
            std::push_heap(this->array_begin, this->array_begin + this->current_size);

            return true;
        }

        return false;
    }


/*
 * Public member function that adds a point to the priority queue of nearest neighbors if the priority queue is below
 * capacity or if the point is closer than an existing neighbor. If the point is closer than an existing neighbor AND
 * the priority queue is at capacity, removes the furthest neighbor before adding the point.
 */
    bool registerAsNeighborIfEligible(float* potential_neighbor) {
        double distance_from_query = distanceBetween(this->query_point, potential_neighbor, this->num_dimensions);

        if (this->registerAsNeighborIfNotFull(potential_neighbor, distance_from_query)) { return true; }

        if (this->registerAsNeighborIfCloser(potential_neighbor, distance_from_query)) { return true; }

        delete[] potential_neighbor;

        return false;
    }
};
