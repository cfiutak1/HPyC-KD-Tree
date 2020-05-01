#ifndef QUERYTHREADPOOL_HPP
#define QUERYTHREADPOOL_HPP


// #include "QueryWorker.hpp"
#include "TaskQueue.hpp"
#include "ResultQueue.hpp"


// #include "../kdtree/KDTree.hpp"
#include "../kdtree/KDTree.hpp"
#include "../kdtree/KNNQueue.hpp"
#include "../kdtree/Point.hpp"
#include "../filedata/QueryFileData.hpp"

#include <vector>
#include <atomic>

class QueryWorker;

class QueryThreadPool {
public:
    KDTree* tree;
    std::vector<Point*>& query_points;
    QueryFileData* file_data;
    unsigned int num_threads = 0;

    unsigned int batch_size = 100;

    TaskQueue* task_queue = nullptr;
    ResultQueue* result_queue = nullptr;
    std::vector<QueryWorker*> workers;
    std::atomic<unsigned int> completed_threads = 0;

    QueryThreadPool(
        KDTree* tree_in,
        std::vector<Point*>& query_points_in,
        QueryFileData* file_data_in,
        unsigned int num_threads_in
    ):
        tree(tree_in),
        query_points(query_points_in),
        file_data(file_data_in),
        num_threads(num_threads_in)
    {
        this->task_queue = new TaskQueue(query_points_in.size(), this->batch_size);
        this->result_queue = new ResultQueue();
        this->workers.reserve(this->num_threads);
    }

    ~QueryThreadPool();

    inline KDTree* getTree() { return this->tree; }
    inline QueryFileData* getFileData() { return this->file_data; }

    inline void registerCompletedThread() {
        this->completed_threads++;
    }

    std::map<uint64_t, std::vector<KNNQueue*>> getResults() {
        return this->result_queue->getResults();
    }

    void run();
    void scheduleWorkers();
    void waitUntilComplete();
};


#endif
