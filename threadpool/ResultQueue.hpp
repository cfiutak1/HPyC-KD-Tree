#ifndef RESULTQUEUE_HPP
#define RESULTQUEUE_HPP

#include "../kdtree/Neighbor.hpp"
#include "../kdtree/KNNQueue.hpp"

#include <map>
#include <vector>


class ResultQueue {
private:
    std::map<uint64_t, std::vector<KNNQueue*>> results;

public:
    ResultQueue() {}

    inline std::map<uint64_t, std::vector<KNNQueue*>> getResults() { return this->results; }
    inline unsigned int getNumCompletedTasks() { return this->results.size(); }

    void registerCompletedTask(
        unsigned int task_begin,
        std::vector<KNNQueue*> new_results)
    {
        this->results.insert({task_begin, new_results});
    }
};

#endif
