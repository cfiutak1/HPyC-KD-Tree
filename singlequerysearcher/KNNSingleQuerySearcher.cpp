#include "KNNSingleQuerySearcher.hpp"

#include "SingleQueryWorker.hpp"


KNNQueue* KNNSingleQuerySearcher::nearestNeighborsSearch() {
    this->scheduleWorkers();
    this->waitUntilComplete();

    return this->nearest_neighbors;
}


void KNNSingleQuerySearcher::scheduleWorkers() {
    std::vector<std::future<void>> futures;
    futures.resize(this->num_threads);

    for (unsigned int i = 0; i < this->num_threads; i++) {
        SingleQueryWorker* worker = new SingleQueryWorker(this);
        worker->id = i;
        this->workers.push_back(worker);
        futures.push_back(std::async(std::launch::async, &SingleQueryWorker::doWork, worker));
    }
}


void KNNSingleQuerySearcher::waitUntilComplete() {
    while (!this->completed) { continue; }
}
