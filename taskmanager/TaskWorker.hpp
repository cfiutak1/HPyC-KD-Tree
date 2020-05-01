#ifndef TASKWORKER_HPP
#define TASKWORKER_HPP

#include <vector>
#include <functional>
#include "TaskQueue.hpp"
#include "Task.hpp"
#include <chrono>

template <typename DataType, typename ResultType, typename TaskType>
class TaskWorker {
private:
    StaticThreadSafeQueue<DataType*>& task_queue;
    std::vector<ResultType*>& result_queue;
    TaskType& operation;


public:
    unsigned int id = 0;
    TaskWorker(StaticThreadSafeQueue<DataType*>& task_queue_in, std::vector<ResultType*>& result_queue_in, TaskType& operation_in, unsigned int id_in):
        task_queue(task_queue_in),
        result_queue(result_queue_in),
        operation(operation_in),
        id(id_in)
    {}

    void doWork() {
        typename std::vector<DataType*>::iterator task = this->task_queue.dequeue();
        // typename std::vector<DataType*>::iterator end = this->task_queue.getEnd();
        //auto query_start = std::chrono::steady_clock::now();
        while (std::distance(task_queue.getBegin(), task) < std::distance(task_queue.getBegin(), task_queue.getEnd())) {
            //printf("%s:%d worker%u pos=%ld %lu\n", __FILE__, __LINE__, this->id, task - task_queue.getBegin(), this->result_queue.size());
            auto t = *task;
            result_queue[task - task_queue.getBegin()] = this->operation(t);
            task = this->task_queue.dequeue();
        }
    //auto query_end = std::chrono::steady_clock::now();
    //std::chrono::duration<double> query_diff = (query_end - query_start);
    //printf("worker%u query took %f seconds\n", id, query_diff.count());
        // printf("%s:%d worker%u ending at %ld\n", __FILE__, __LINE__, this->id, task - task_queue.getBegin());
    }
};

#endif
