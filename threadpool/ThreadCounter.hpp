#ifndef THREADCOUNTER_HPP
#define THREADCOUNTER_HPP

#include <mutex>


class ThreadCounter {
private:
    unsigned int num_threads;
    unsigned int num_available_threads;
    std::mutex mtx;

public:
    ThreadCounter() {}
    ThreadCounter(unsigned int num_threads):
        num_threads(num_threads),
        num_available_threads(num_threads)
    {}

    bool requestThread() {
        std::lock_guard<std::mutex> lock(this->mtx);

        if (this->num_available_threads != 0) {
            this->num_available_threads--;

            return true;
        }

        return false;
    }

    bool requestThreads(unsigned int num_requested_threads) {
        std::lock_guard<std::mutex> lock(this->mtx);

        if (num_requested_threads <= this->num_available_threads) {
            this->num_available_threads -= num_requested_threads;

            return true;
        }

        return false;
    }

    void returnThread() {
        std::lock_guard<std::mutex> lock(this->mtx);
        this->num_available_threads++;
    }

    void returnThreads(unsigned int num_returned_threads) {
        std::lock_guard<std::mutex> lock(this->mtx);
        this->num_available_threads += num_returned_threads;
    }
};


#endif
