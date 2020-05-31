#ifndef KNNQUEUE_HPP
#define KNNQUEUE_HPP

#include "Neighbor.hpp"
//#include "Point.hpp"
//#include "float.hpp"

#include <mutex>
#include <queue>
#include <vector>



class KNNQueue {
private:
//    typedef std::priority_queue<Neighbor, std::vector<Neighbor>, NeighborComparator> super;
    const static NeighborComparator comp;
    std::priority_queue<Neighbor, std::vector<Neighbor>, NeighborComparator> queue;
    float* query_point;
    const uint64_t& num_neighbors;
    const uint64_t& num_dimensions;

//    std::mutex mtx;

    bool closerThanFarthestNeighbor(const double& p);

public:
    KNNQueue() = delete;
    KNNQueue(float* query_point_in, const uint64_t& num_neighbors_in, const uint64_t& num_dimensions_in):
        query_point(query_point_in),
        num_neighbors(num_neighbors_in),
        num_dimensions(num_dimensions_in),
        queue(comp)
    {}

    ~KNNQueue() {}


//    std::vector<Neighbor> getNeighbors() { return this->c; }
    inline bool empty() const { return this->queue.empty(); }
    inline void pop() { this->queue.pop(); }
    inline Neighbor top() { return this->queue.top(); }

    inline bool isFull() { return this->queue.size() == this->num_neighbors; }

    bool registerAsNeighborIfCloser(float* potential_neighbor);
//    bool registerAsNeighborIfCloserTS(float* potential_neighbor);
};

#endif
