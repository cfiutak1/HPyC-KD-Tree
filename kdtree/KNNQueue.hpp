#ifndef KNNQUEUE_HPP
#define KNNQUEUE_HPP

#include "Neighbor.hpp"
#include "Point.hpp"

#include <mutex>
#include <queue>
#include <vector>



class KNNQueue : public std::priority_queue<Neighbor, std::vector<Neighbor>, NeighborComparator> {
private:
    typedef std::priority_queue<Neighbor, std::vector<Neighbor>, NeighborComparator> super;

    Point* query_point;
    uint64_t num_neighbors;

    std::mutex mtx;

    bool closerThanFarthestNeighbor(Point* p);

public:
    KNNQueue() {}
    KNNQueue(Point* query_point_in, uint64_t num_neighbors_in):
        query_point(query_point_in),
        num_neighbors(num_neighbors_in)
    {}

    ~KNNQueue() {}


    std::vector<Neighbor> getNeighbors() { return this->c; }

    inline bool isFull() { return super::size() == this->num_neighbors; }

    bool registerAsNeighborIfCloser(Point* potential_neighbor);
    bool registerAsNeighborIfCloserTS(Point* potential_neighbor);
};

#endif
