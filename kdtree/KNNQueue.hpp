#include <queue>
#include <vector>
#include <cstdint>
#include <mutex>

#pragma once


class Neighbor {
public:
    float* point;
    double distance_from_queried_point;

    Neighbor() = delete;

    Neighbor(float* point_in, double distance_from_queried_point_in):
            point(point_in),
            distance_from_queried_point(distance_from_queried_point_in)
    {}
};


class NeighborComparator {
public:
    NeighborComparator() = default;

    bool operator()(const Neighbor& n1, const Neighbor& n2) {
        return n1.distance_from_queried_point < n2.distance_from_queried_point;
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
    std::priority_queue<Neighbor, std::vector<Neighbor>, NeighborComparator> nearest_neighbors;

    friend class ThreadSafeKNNQueue;

    inline bool closerThanFarthestNeighbor(const double& p) const {
        return p < this->top().distance_from_queried_point;
    }

public:
    KNNQueue() {}

    KNNQueue(const float* query_point_in, const uint64_t& num_neighbors_in, const uint64_t& num_dimensions_in):
        query_point(query_point_in),
        num_neighbors(num_neighbors_in),
        num_dimensions(num_dimensions_in),
        nearest_neighbors(NeighborComparator{})
    {}

    inline bool empty() const {
        return this->nearest_neighbors.empty();
    }

    inline void pop() {
        this->nearest_neighbors.pop();
    }

    inline void removeFarthestNeighbor() {
        delete[] this->nearest_neighbors.top().point;
        this->nearest_neighbors.pop();
    }

    inline Neighbor top() const {
        return this->nearest_neighbors.top();
    }

    inline bool full() const {
        return this->nearest_neighbors.size() == this->num_neighbors;
    }

    inline bool registerAsNeighborIfNotFull(float* potential_neighbor, double distance_from_query) {
        // If the priority queue is below capacity, add the potential neighbor regardless of its distance to the query point.
        if (!this->full()) {
            this->nearest_neighbors.push({potential_neighbor, distance_from_query});

            return true;
        }

        return false;
    }

    inline bool registerAsNeighborIfCloser(float* potential_neighbor, double distance_from_potential_query) {
        // If the priority queue is at capacity and the potential neighbor is closer to the query point than the current
        // furthest neighbor, remove the furthest neighbor from the priority queue and push the potential neighbor.
        if (this->closerThanFarthestNeighbor(distance_from_potential_query)) {
            this->removeFarthestNeighbor();
            this->nearest_neighbors.push(Neighbor(potential_neighbor, distance_from_potential_query));

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
