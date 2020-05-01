#ifndef TASKQUEUE_HPP
#define TASKQUEUE_HPP

#include <mutex>
#include <atomic>
#include <vector>

template <typename T>
class StaticThreadSafeQueue {
private:
    typedef typename std::vector<T>::iterator iter_t;

    iter_t begin;
    iter_t end;

    unsigned long size;

    std::atomic<unsigned long> task_index = 0;


public:
    StaticThreadSafeQueue(iter_t begin_in, iter_t end_in):
        begin(begin_in),
        end(end_in),
        size(end_in - begin_in)
    {}

    // ~StaticThreadSafeQueue() {
    //     iter_t temp = begin;
    //
    //     while (temp != end) {
    //         delete *temp;
    //         ++temp;
    //     }
    // }

    inline iter_t getBegin() const { return this->begin; }
    inline iter_t getEnd() const { return this->end; }

    inline unsigned long getSize() const { return this->size; }

    iter_t dequeue() {
        unsigned long i = task_index++;

        // printf("%s:%d size =%lu i=%lu\n", __FILE__, __LINE__, this->size, i);

        if (i >= this->size) return this->end;

        return this->begin + i;
    }
};


#endif
