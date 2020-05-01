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
    std::vector<DataType*> tasks;
    unsigned int num_threads;
    unsigned int num_queues;

    std::vector<StaticThreadSafeQueue<DataType*>*> task_queues;
    std::vector<std::vector<KNNQueue*>> result_queues;

    inline void initializeQueues() {
        this->num_queues = 1;
        //this->num_queues = std::max(1, int(pow(2, std::floor(log2(this->num_threads)) - 1)));
        this->task_queues.reserve(num_queues);
        this->result_queues.reserve(num_queues);

        unsigned long max_chunk_size = std::ceil(float(this->tasks.size()) / this->num_queues);

        //printf("%s:%d num_queues=%u, max_chunk_size=%lu\n", __FILE__, __LINE__, num_queues, max_chunk_size);

        for (unsigned int i = 0; i < num_queues - 1; ++i) {
            StaticThreadSafeQueue<DataType*>* tq = new StaticThreadSafeQueue<DataType*>(
                this->tasks.begin() + (i * max_chunk_size),
                this->tasks.begin() + ((i + 1) * max_chunk_size)
            );
            this->task_queues.push_back(tq);
            //printf("%s:%d %lu -> %lu\n", __FILE__, __LINE__, i * max_chunk_size, (i + 1) * max_chunk_size);
            std::vector<ResultType*> rq;
            // printf("%s:%d rqsize=%lu\n", __FILE__, __LINE__, rq->size());
            rq.resize(max_chunk_size);
            // printf("%s:%d rqsize=%lu\n", __FILE__, __LINE__, rq->size());
            this->result_queues.push_back(rq);
        }

        StaticThreadSafeQueue<DataType*>* tq = new StaticThreadSafeQueue<DataType*>(
            this->tasks.begin() + ((num_queues - 1) * max_chunk_size),
            this->tasks.end()
        );
        this->task_queues.push_back(tq);

        //printf("%s:%d %lu -> %lu\n", __FILE__, __LINE__, (num_queues - 1) * max_chunk_size, this->tasks.size());
        //printf("%s:%d last chunk size = %lu\n", __FILE__, __LINE__, tq->getSize());

        std::vector<ResultType*> rq;
        rq.resize(tq->getSize());
        this->result_queues.push_back(rq);
    }

    unsigned int threads_to_queue = 4;




public:
    TaskManager(std::vector<DataType*> tasks_in, unsigned int num_threads_in):
        tasks(tasks_in),
        num_threads(num_threads_in)
    {
        // printf("%s:%d\n", __FILE__, __LINE__);
        this->initializeQueues();
        // printf("%s:%d\n", __FILE__, __LINE__);
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
            // printf("%s:%d Worker %lu attached to queues %lu\n", __FILE__, __LINE__, i, i % this->num_queues);
            TaskWorker<DataType, ResultType, TaskType> w(
                *(this->task_queues[i % this->num_queues]),
                this->result_queues[i % this->num_queues],
                operation, i
            );
            // printf("%s:%d Created worker with id %u\n", __FILE__, __LINE__, w.id);
            workers.push_back(w);
        }


        std::vector<std::thread> threads;
        threads.reserve(this->num_threads);

        for (TaskWorker<DataType, ResultType, TaskType>& worker : workers) {
            std::thread t(&TaskWorker<DataType, ResultType, TaskType>::doWork, &worker);
            threads.push_back(std::move(t));
        }
        // printf("%s:%d\n", __FILE__, __LINE__);

        for (std::thread& t : threads) {
            if (t.joinable()) t.join();
        }
        // printf("%s:%d\n", __FILE__, __LINE__);

        return this->result_queues;
    }
};

#endif
