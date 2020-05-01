#ifndef TASKMANAGER_HPP
#define TASKMANAGER_HPP

#include <vector>
#include <cmath>
#include <thread>
#include <cstdio>

#include "../kdtree/KNNQueue.hpp"

#include "TaskQueue.hpp"
#include "TaskWorker.hpp"


template <typename DataType, typename ResultType>
class TaskManager {
private:
    const unsigned int threads_to_queue = 4;
    std::vector<DataType*> tasks;
    unsigned int num_threads;
    unsigned int num_queues;

    std::vector<StaticThreadSafeQueue<DataType*>*> task_queues;
    std::vector<std::vector<KNNQueue*>> result_queues;

    inline void initializeQueues() {
        this->num_queues = 1;
        this->task_queues.reserve(num_queues);
        this->result_queues.reserve(num_queues);

        unsigned long max_chunk_size = std::ceil(float(this->tasks.size()) / this->num_queues);

        for (unsigned int i = 0; i < num_queues - 1; ++i) {
            StaticThreadSafeQueue<DataType*>* tq = new StaticThreadSafeQueue<DataType*>(
                this->tasks.begin() + (i * max_chunk_size),
                this->tasks.begin() + ((i + 1) * max_chunk_size)
            );
            this->task_queues.push_back(tq);
            std::vector<ResultType*> rq;
            rq.resize(max_chunk_size);
            this->result_queues.push_back(rq);
        }

        StaticThreadSafeQueue<DataType*>* tq = new StaticThreadSafeQueue<DataType*>(
            this->tasks.begin() + ((num_queues - 1) * max_chunk_size),
            this->tasks.end()
        );
        this->task_queues.push_back(tq);

        std::vector<ResultType*> rq;
        rq.resize(tq->getSize());
        this->result_queues.push_back(rq);
    }

public:
    TaskManager(std::vector<DataType*> tasks_in, unsigned int num_threads_in):
        tasks(tasks_in),
        num_threads(num_threads_in)
    {
        this->initializeQueues();
    }

    ~TaskManager() {
        for (auto tq : this->task_queues) {
            delete tq;
        }
    }

    template <typename TaskType>
    std::vector<std::vector<ResultType*>>& completeTasks(TaskType& operation) {
        std::vector<TaskWorker<DataType, ResultType, TaskType>> workers;

        for (unsigned int i = 0; i < this->num_threads; ++i) {
            TaskWorker<DataType, ResultType, TaskType> w(
                *(this->task_queues[i % this->num_queues]),
                this->result_queues[i % this->num_queues],
                operation, i
            );

            workers.push_back(w);
        }

        std::vector<std::thread> threads;
        threads.reserve(this->num_threads);

        for (TaskWorker<DataType, ResultType, TaskType>& worker : workers) {
            std::thread t(&TaskWorker<DataType, ResultType, TaskType>::doWork, &worker);
            threads.push_back(std::move(t));
        }

        for (std::thread& t : threads) {
            if (t.joinable()) t.join();
        }

        return this->result_queues;
    }
};

#endif
