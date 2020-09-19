#include "../kdtree/KNNQueue.hpp"

#include <queue>
#include <vector>
#include <cstdint>
#include <mutex>

#pragma once


class ThreadSafeKNNQueue {
private:
    friend class ParallelKDTree;

    const float* query_point;
    uint64_t num_neighbors;
    uint64_t num_dimensions;

    std::mutex mtx;


public:
    KNNQueue queue;

    ThreadSafeKNNQueue() = default;

    ThreadSafeKNNQueue(const float* query_point_in, const uint64_t& num_neighbors_in, const uint64_t& num_dimensions_in):
        query_point(query_point_in),
        num_neighbors(num_neighbors_in),
        num_dimensions(num_dimensions_in),
        queue(query_point, num_neighbors, num_dimensions)
    {}

    ThreadSafeKNNQueue(const ThreadSafeKNNQueue& k) {
        this->query_point = k.query_point;
        this->num_neighbors = k.num_neighbors;
        this->num_dimensions = k.num_dimensions;
        this->queue = k.queue;
    }

    ThreadSafeKNNQueue& operator =(const ThreadSafeKNNQueue& k) {
        this->query_point = k.query_point;
        this->num_neighbors = k.num_neighbors;
        this->num_dimensions = k.num_dimensions;
        this->queue = k.queue;

        return *this;
    }


/*
 * Public member function that adds a point to the priority queue of nearest neighbors if the priority queue is below
 * capacity or if the point is closer than an existing neighbor. If the point is closer than an existing neighbor AND
 * the priority queue is at capacity, removes the furthest neighbor before adding the point.
 */
    bool registerAsNeighborIfEligible(float* potential_neighbor) {
        double distance_from_query = distanceBetween(this->query_point, potential_neighbor, this->num_dimensions);

        this->mtx.lock();

        if (this->queue.registerAsNeighborIfNotFull(potential_neighbor, distance_from_query)) {
            this->mtx.unlock();

            return true;
        }

        if (this->queue.registerAsNeighborIfCloser(potential_neighbor, distance_from_query)) {
            this->mtx.unlock();

            return true;
        }

        this->mtx.unlock();

        delete[] potential_neighbor;

        return false;
    }
};
