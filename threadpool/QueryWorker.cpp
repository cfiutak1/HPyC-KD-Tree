#include "QueryWorker.hpp"

#include "../kdtree/KNNSearcher.hpp"
#include <chrono>


void QueryWorker::processTasks() {
    while (true) {
        Task* task = this->task_queue->requestTask();

        if (task == nullptr) break;

        if (task->begin == task->end) {
            delete task;
            break;
        }

        std::vector<KNNQueue*> results;
        results.reserve(task->end - task->begin);

        for (unsigned int i = task->begin; i < task->end; i++) {
            KNNSearcher searcher(this->thread_pool->tree, this->thread_pool->file_data, this->thread_pool->query_points[i]);
            results.push_back(searcher.nearestNeighborsSearch());

        }

        this->result_queue->registerCompletedTask(task->begin, results);

        delete task;
    }

    this->thread_pool->registerCompletedThread();
}
