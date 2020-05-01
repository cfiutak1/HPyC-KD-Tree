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
    KNNQueue() {
        // printf("%s:%d Default constructor called\n", __FILE__, __LINE__);
    }
    KNNQueue(Point* q, uint64_t n) : query_point(q), num_neighbors(n) {
        // printf("%s:%d Constructor n=%d called\n", __FILE__, __LINE__, n);
    }

    ~KNNQueue() {
        // printf("%s:%d Destructor called\n", __FILE__, __LINE__);
    }


    std::vector<Neighbor> getNeighbors() { return this->c; }

    inline bool isFull() { return super::size() == this->num_neighbors; }

    bool registerAsNeighborIfCloser(Point* potential_neighbor);
    bool registerAsNeighborIfCloserTS(Point* potential_neighbor);
};

#endif
