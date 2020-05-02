#ifndef KNNQUEUE_HPP
#define KNNQUEUE_HPP

#include "Neighbor.hpp"
//#include "Point.hpp"
#include "KDNode.hpp"

#include <mutex>
#include <queue>
#include <vector>



class KNNQueue : public std::priority_queue<Neighbor, std::vector<Neighbor>, NeighborComparator> {
private:
    typedef std::priority_queue<Neighbor, std::vector<Neighbor>, NeighborComparator> super;

    KDNode* query_point;
    uint64_t num_neighbors;

    std::mutex mtx;

    bool closerThanFarthestNeighbor(KDNode* p);

public:
    KNNQueue() {}
    KNNQueue(KDNode* query_point_in, uint64_t num_neighbors_in):
        query_point(query_point_in),
        num_neighbors(num_neighbors_in)
    {}

    ~KNNQueue() {}


    std::vector<Neighbor> getNeighbors() { return this->c; }

    inline bool isFull() { return super::size() == this->num_neighbors; }

    bool registerAsNeighborIfCloser(KDNode* potential_neighbor);
    bool registerAsNeighborIfCloserTS(KDNode* potential_neighbor);
};

#endif
