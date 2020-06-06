#include "KNNQueue.hpp"


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


inline bool KNNQueue::closerThanFarthestNeighbor(const double& p) const {
    return p < this->top().distance_from_queried_point;
}


/*
 * Public member function that adds a point to the priority queue of nearest neighbors if the priority queue is below
 * capacity or if the point is closer than an existing neighbor. If the point is closer than an existing neighbor AND
 * the priority queue is at capacity, removes the furthest neighbor before adding the point.
 */
bool KNNQueue::registerAsNeighborIfCloser(float* potential_neighbor) {
    // If the priority queue is below capacity, add the potential neighbor regardless of its distance to the query point.
    if (!this->isFull()) {
        double distance = distanceBetween(this->query_point, potential_neighbor, this->num_dimensions);

        this->nearest_neighbors.push(Neighbor(potential_neighbor, distance));

        return true;
    }

    double distance = distanceBetween(this->query_point, potential_neighbor, this->num_dimensions);

    // If the priority queue is at capacity and the potential neighbor is closer to the query point than the current
    // furthest neighbor, remove the furthest neighbor from the priority queue and push the potential neighbor.
    if (this->closerThanFarthestNeighbor(distance)) {
        this->nearest_neighbors.pop();
        this->nearest_neighbors.push(Neighbor(potential_neighbor, distance));

        return true;
    }

    return false;
}
