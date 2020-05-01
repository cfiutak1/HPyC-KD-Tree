#ifndef TASKQUEUE_HPP
#define TASKQUEUE_HPP

#include <vector>
#include <mutex>
#include <cmath>

struct Task {
    uint64_t begin = 0;
    uint64_t end = 0;
};


class TaskQueue {
private:
    uint64_t num_queries = 0;
    unsigned int batch_size = 0;

    std::vector<Task*> tasks;

    std::mutex mutex;

public:
    TaskQueue(uint64_t num_queries_in, unsigned int batch_size_in):
        num_queries(num_queries_in),
        batch_size(batch_size_in)
    {
        unsigned int num_jobs = std::ceil(((float) num_queries_in) / this->batch_size);

        this->tasks.reserve(num_jobs);

        for (unsigned int i = 0; i < num_jobs - 1; i++) {
            tasks.push_back(new Task{i * this->batch_size, (i + 1) * batch_size});
        }

        tasks.push_back(new Task{(num_jobs - 1) * this->batch_size, this->num_queries});
    }

    inline unsigned int getNumTasks() { return this->tasks.size(); }

    ~TaskQueue() {
        for (auto task : tasks) delete task;
    }

    Task* requestTask() {
        std::lock_guard<std::mutex> lock(this->mutex);

        if (this->tasks.size() == 0) return nullptr;

        Task* task = this->tasks.back();
        this->tasks.pop_back();

        return task;
    }
};

#endif
