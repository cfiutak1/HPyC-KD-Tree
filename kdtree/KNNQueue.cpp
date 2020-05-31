#include "KNNQueue.hpp"


inline double distanceBetween(const float* p1, const float* p2, const int& size) {
    double distance = 0.0;

    for (int i = 0; i < size; ++i) {
        double diff = p2[i] - p1[i];
        distance += (diff * diff);
    }

    return distance;
}


inline bool KNNQueue::closerThanFarthestNeighbor(const double& p) {
    return p < this->top().distance_from_queried_point;
}


bool KNNQueue::registerAsNeighborIfCloser(float* potential_neighbor) {
    if (!this->isFull()) {
        double distance = distanceBetween(this->query_point, potential_neighbor, this->num_dimensions);

        Neighbor new_neighbor(potential_neighbor, distance);
        this->queue.push(new_neighbor);

        return true;
    }


    double distance = distanceBetween(this->query_point, potential_neighbor, this->num_dimensions);

    if (this->closerThanFarthestNeighbor(distance)) {
        Neighbor new_neighbor(potential_neighbor, distance);

        this->queue.pop();
        this->queue.push(new_neighbor);

        return true;
    }

    return false;
}


