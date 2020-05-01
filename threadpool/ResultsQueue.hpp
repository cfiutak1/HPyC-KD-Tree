#ifndef RESULTSQUEUE_HPP
#define RESULTSQUEUE_HPP

#include "../kdtree/Neighbor.hpp"

#include <map>
#include <vector>


class ResultsQueue {
private:
    std::map<uint64_t, std::vector<std::vector<Neighbor>>> results;

public:
    unsigned int getNumCompletedTasks() { return this->results.size(); }

    void registerCompletedTask(unsigned int task_begin, std::vector<std::vector<Neighbor>> results) {
        results.insert(task_begin, results);
    }
};

#endif
