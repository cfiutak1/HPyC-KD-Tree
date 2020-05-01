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
    TaskWorker(
        StaticThreadSafeQueue<DataType*>& task_queue_in,
        std::vector<ResultType*>& result_queue_in,
        TaskType& operation_in,
        unsigned int id_in
    ):
        task_queue(task_queue_in),
        result_queue(result_queue_in),
        operation(operation_in),
        id(id_in)
    {}

    void doWork() {
        typename std::vector<DataType*>::iterator task = this->task_queue.dequeue();

        while (std::distance(task_queue.getBegin(), task) < std::distance(task_queue.getBegin(), task_queue.getEnd())) {
            auto t = *task;
            result_queue[task - task_queue.getBegin()] = this->operation(t);
            task = this->task_queue.dequeue();
        }
    }
};

#endif
