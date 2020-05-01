#ifndef QUERYWORKER_HPP
#define QUERYWORKER_HPP

#include "QueryThreadPool.hpp"
#include "TaskQueue.hpp"
#include "ResultQueue.hpp"
#include "../kdtree/KNNQueue.hpp"


class QueryWorker {
private:
    QueryThreadPool* thread_pool;
    TaskQueue* task_queue;
    ResultQueue* result_queue;


public:
    unsigned int id = 0;
    
    QueryWorker(
        QueryThreadPool* thread_pool_in,
        TaskQueue* task_queue_in,
        ResultQueue* result_queue_in
    ):
        thread_pool(thread_pool_in),
        task_queue(task_queue_in),
        result_queue(result_queue_in)
    {}

    void processTasks();
};

#endif
