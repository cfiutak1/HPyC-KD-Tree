#include "QueryThreadPool.hpp"

#include "QueryWorker.hpp"

#include <future>


QueryThreadPool::~QueryThreadPool() {
    delete task_queue;
    delete result_queue;

    for (auto worker : this->workers) delete worker;
}


void QueryThreadPool::run() {
    this->scheduleWorkers();
    this->waitUntilComplete();
}


void QueryThreadPool::scheduleWorkers() {
    std::vector<std::future<void>> futures;
    futures.resize(this->num_threads);
    for (unsigned int i = 0; i < this->num_threads; i++) {
        QueryWorker* worker = new QueryWorker(
            this,
            this->task_queue,
            this->result_queue
        );
        worker->id = i;
        this->workers.push_back(worker);
        futures.push_back(std::async(std::launch::async, &QueryWorker::processTasks, worker));
    }
}


void QueryThreadPool::waitUntilComplete() {
    while (this->completed_threads != this->num_threads) continue;
}
