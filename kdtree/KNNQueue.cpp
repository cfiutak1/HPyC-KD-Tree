#include "KNNQueue.hpp"


bool KNNQueue::closerThanFarthestNeighbor(Point* p) {
    return this->query_point->distanceBetween(p) < this->top().distance_from_queried_point;
}


bool KNNQueue::registerAsNeighborIfCloser(Point* potential_neighbor) {
    if (!this->isFull()) {
        double distance = this->query_point->distanceBetween(potential_neighbor);

        Neighbor new_neighbor(potential_neighbor, distance);
        super::push(new_neighbor);

        return true;
    }


    if (this->closerThanFarthestNeighbor(potential_neighbor)) {
        double distance = this->query_point->distanceBetween(potential_neighbor);
        Neighbor new_neighbor(potential_neighbor, distance);

        super::pop();
        super::push(new_neighbor);

        return true;
    }

    return false;
}


bool KNNQueue::registerAsNeighborIfCloserTS(Point* potential_neighbor) {
    if (!this->isFull()) {
        std::lock_guard<std::mutex> lock(this->mtx);

        if (!this->isFull()) {
            double distance = this->query_point->distanceBetween(potential_neighbor);

            Neighbor new_neighbor(potential_neighbor, distance);
            super::push(new_neighbor);

            return true;
        }
    }

    if (this->closerThanFarthestNeighbor(potential_neighbor)) {
        std::lock_guard<std::mutex> lock(this->mtx);

        if (this->closerThanFarthestNeighbor(potential_neighbor)) {
            double distance = this->query_point->distanceBetween(potential_neighbor);
            Neighbor new_neighbor(potential_neighbor, distance);

            super::pop();
            super::push(new_neighbor);

            return true;
        }
    }

    return false;
}
