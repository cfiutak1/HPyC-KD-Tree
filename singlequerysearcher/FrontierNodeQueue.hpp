#ifndef FRONTIERNODEQUEUE_HPP
#define FRONTIERNODEQUEUE_HPP

#include <vector>
#include <stack>
#include <queue>
#include <mutex>
#include "../kdtree/KDNode.hpp"


class FrontierNodeQueue {
private:
    std::queue<KDNode*> tasks;

    std::mutex mutex;

public:
    FrontierNodeQueue(KDNode* root) {
        this->tasks.push(root);
    }

    bool tasksRemain() { return this->tasks.size() > 0; }

    KDNode* requestNode() {
        std::lock_guard<std::mutex> lock(this->mutex);

        if (this->tasks.empty()) return nullptr;

        // TODO explore performance of FIFO vs LIFO
        KDNode* node_to_explore = this->tasks.front();
        this->tasks.pop();

        return node_to_explore;
    }

    void enqueue(KDNode* node) {
        std::lock_guard<std::mutex> lock(this->mutex);
        this->tasks.push(node);
    }
};


#endif
