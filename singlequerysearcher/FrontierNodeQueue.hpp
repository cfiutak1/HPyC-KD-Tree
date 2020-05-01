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
        // printf("%s:%d\n", __FILE__, __LINE__);
        std::lock_guard<std::mutex> lock(this->mutex);
        // printf("%s:%d Node requested, size is %lu\n", __FILE__, __LINE__, this->tasks.size());
        if (this->tasks.size() == 0) return nullptr;


        // TODO explore performance of FIFO vs LIFO
        KDNode* node_to_explore = this->tasks.front();
        // assert(node_to_explore != nullptr);
        this->tasks.pop();

        // assert(node_to_explore != nullptr);

        return node_to_explore;
    }

    void enqueue(KDNode* node) {
        std::lock_guard<std::mutex> lock(this->mutex);
        this->tasks.push(node);
        // printf("%s:%d Added item, new size %lu\n", __FILE__, __LINE__, this->tasks.size());
    }
};


#endif
