#include "Neighbor.hpp"

#include <cstdint>
#include <algorithm>

#pragma once


inline std::size_t highestSetBitPosition(std::size_t index) {
    return (sizeof(std::size_t) * 8 - 1) - __builtin_clzl(index);
}


inline std::size_t getLevel(std::size_t index) {
    return highestSetBitPosition(index + 1);
}


inline bool onMaxLevel(std::size_t index) {
    return highestSetBitPosition(index + 1) & 1u;
}


inline std::size_t getParentIndex(std::size_t index) {
    return (index - 1) / 2;
}


/*
 * Free function that computes the euclidean squared distance between two float arrays of equal size.
 */
inline double distanceBetween(const float* p1, const float* p2, const int& size) {
    double distance = 0.0;

    for (int i = 0; i < size; ++i) {
        double diff = p2[i] - p1[i];
        distance += (diff * diff);
    }

    return distance;
}



class KNNQueue {
private:
    const float* query_point;
    std::size_t capacity;
    uint64_t num_dimensions;
    Neighbor* array;
    std::size_t current_size = 0;

    inline std::size_t findDescendantOneChild(std::size_t index, bool on_max_level) const;

    inline std::size_t findDescendantBothChildrenNoGrandchildren(std::size_t index, bool on_max_level) const;

    std::size_t findDescendantBothChildrenSomeGrandchildren(std::size_t index, bool on_max_level) const;

    std::size_t findDescendantFullFamily(std::size_t index, bool on_max_level) const;

    void pushDownOneChild(std::size_t index, bool on_max_level);

    void pushDownBothChildrenNoGrandchildren(std::size_t index, bool on_max_level);

    template <std::size_t (KNNQueue::*FindDescendant)(std::size_t, bool) const>
    // TODO When searching, need to skip child levels
    inline std::size_t pushDownTwoLevels(std::size_t index, bool on_max_level) {
        // Find the most extreme (greatest or least, depending on the level) value's index, and then move it to the
        // index-th position.
        std::size_t descendant = (this->*FindDescendant)(index, on_max_level);

        std::swap(this->array[index], this->array[descendant]);

        // Swaps the current item at a[descendant] with its parent, if necessary
        std::size_t descendant_parent = getParentIndex(descendant);
        bool swap_desc = ((this->array[descendant_parent] < this->array[descendant]) ^ on_max_level) && (descendant > (2 * index + 2));

        std::swap(
            this->array[descendant],
            this->array[descendant - ((descendant - descendant_parent) & (0u - swap_desc))]
        );

        return descendant;
    }


    inline void pushDownIndefinitely(std::size_t index, std::size_t first_index_with_no_grandchildren, std::size_t first_index_with_no_children, bool on_max_level) {
        while (index < first_index_with_no_grandchildren) {
            std::size_t new_index = pushDownTwoLevels<&KNNQueue::findDescendantFullFamily>(index, on_max_level);

            on_max_level ^= ((new_index == 2 * index + 1) || (new_index == 2 * index + 2));

            if (new_index == index) return;
            index = new_index;
        }

        if (index == first_index_with_no_grandchildren) {
            if (4 * index + 6 < this->current_size) {
                pushDownTwoLevels<&KNNQueue::findDescendantFullFamily>(index, on_max_level);
            }

            else {
                pushDownTwoLevels<&KNNQueue::findDescendantBothChildrenSomeGrandchildren>(index, on_max_level);
            }
        }

        else {
            if (index < first_index_with_no_children) {
                this->pushDownBothChildrenNoGrandchildren(index, on_max_level);
            }

            else if ((index == first_index_with_no_children) && (this->current_size % 2 == 0)) {
                this->pushDownOneChild(index, on_max_level);
            }
        }
    }

    inline void pushDownIndefinitely(std::size_t index, bool on_max_level) {
        std::size_t first_index_with_no_descendants = this->current_size / 2;
        std::size_t first_index_with_no_grandchildren = this->current_size / 4;

        this->pushDownIndefinitely(
            index,
            (this->current_size / 4) + ((4 * first_index_with_no_grandchildren) + 3 < this->current_size),
            first_index_with_no_descendants - (first_index_with_no_descendants % 2 == 0),
            on_max_level
        );
    }

    inline bool closerThanFarthestNeighbor(const double& p) const {
        return p < this->array[0].distance_from_queried_point;
    }

    friend class ThreadSafeKNNQueue;


public:
    KNNQueue() = default;

    KNNQueue(const float* query_point_in, const uint64_t& num_neighbors_in, const uint64_t& num_dimensions_in):
        query_point(query_point_in),
        capacity(num_neighbors_in),
        num_dimensions(num_dimensions_in)
    {
        this->array = new Neighbor[num_neighbors_in];
    }

    ~KNNQueue() {
        delete[] this->array;
    }

    inline bool empty() const {
        return this->current_size == 0;
    }

    inline bool full() const {
        return this->current_size == this->capacity;
    }

    inline double getFarthestDistance() const {
        return this->array[0].distance_from_queried_point;
    }

    // TODO need to test pop functions on small inputs
    inline void popMin() {
        std::swap(this->array[0], this->array[this->current_size - 1]);
        this->pushDownIndefinitely(0, false);
        --this->current_size;
    }

    inline void popMax() {
        if (this->current_size < 3) {
            --this->current_size;
            return;
        }

        std::size_t largest_item = 1 + (this->array[1] < this->array[2]);

        std::swap(this->array[largest_item], this->array[this->current_size - 1]);

        this->pushDownIndefinitely(largest_item, true);

        --this->current_size;
    }

    Neighbor& getMax() const {
        if (this->current_size < 3) {
            return this->array[this->current_size - 1];
        }
        return this->array[1 + (this->array[1] < this->array[2])];
    }

    void makeHeap() {
        std::size_t first_index_with_no_descendants = this->current_size / 2;
        std::size_t first_index_with_no_grandchildren = this->current_size / 4;

        first_index_with_no_grandchildren += ((4 * first_index_with_no_grandchildren) + 3 < this->current_size);

        // If the size is 0 or 1, we're done
        if (first_index_with_no_descendants == 0) { return; }

        // If there is an even number of elements, there is one element that only has one child. This handles that case
        // so that the main loop doesn't have to check if an index only has one child.
        if (this->current_size % 2 == 0) {
            --first_index_with_no_descendants;

            this->pushDownOneChild(first_index_with_no_descendants, onMaxLevel(first_index_with_no_descendants));

            // If the size is 2 or 3, we're done
            if (this->current_size < 4) { return; }
        }

        std::size_t current_level = getLevel(first_index_with_no_descendants - 1);
        bool on_max_level = current_level % 2;
        std::size_t current_level_begin = (1u << current_level) - 1;

        // Handle all indices with children but no grandchildren
        for (std::size_t i = first_index_with_no_descendants; i > first_index_with_no_grandchildren; --i) {
            this->pushDownBothChildrenNoGrandchildren(i - 1, on_max_level ^ ((i - 1) < current_level_begin));
        }

        // If the last index that could have grandchildren is missing some, handle it
        if (4 * (first_index_with_no_grandchildren - 1) + 6 >= this->current_size) {
            --first_index_with_no_grandchildren;
            this->pushDownTwoLevels<&KNNQueue::findDescendantBothChildrenSomeGrandchildren>(first_index_with_no_grandchildren, onMaxLevel(first_index_with_no_grandchildren));
        }

        if (this->current_size < 7) { return; }

        // Handle the indices that have a full set of grandchildren and no great+grandchildren
        current_level = getLevel(first_index_with_no_grandchildren);
        on_max_level = current_level % 2;
        std::size_t third_to_last_level_begin = (1u << current_level) - 1;

        for (std::size_t i = first_index_with_no_grandchildren; i > third_to_last_level_begin; --i) {
            this->pushDownTwoLevels<&KNNQueue::findDescendantFullFamily>(i - 1, on_max_level);
        }

        for (std::size_t i = current_level; i > 0; --i) {
            // i will be one level higher than the actual level that's being looped over
            on_max_level = (i % 2 == 0);

            for (std::size_t j = (1u << i) - 1; j >= (1u << (i - 1)); --j) {
                this->pushDownIndefinitely(j - 1, first_index_with_no_grandchildren, first_index_with_no_descendants, on_max_level);
            }
        }
    }

    inline void validate();


    inline bool registerAsNeighborIfNotFull(float* potential_neighbor, double distance_from_query) {
        // If the priority queue is below capacity, add the potential neighbor regardless of its distance to the query point.
        if (!this->full()) {
            this->array[this->current_size].point = potential_neighbor;
            this->array[this->current_size].distance_from_queried_point = distance_from_query;

            ++this->current_size;

            if (this->current_size == this->capacity) {
                std::make_heap(this->array, this->array + this->capacity);
            }

            return true;
        }

        return false;
    }

    inline bool registerAsNeighborIfCloser(float* potential_neighbor, double distance_from_potential_query) {
        // If the priority queue is at capacity and the potential neighbor is closer to the query point than the current
        // furthest neighbor, remove the furthest neighbor from the priority queue and push the potential neighbor.
        if (this->closerThanFarthestNeighbor(distance_from_potential_query)) {
            delete[] this->array[0].point;
            this->array[0].point = potential_neighbor;
            this->array[0].distance_from_queried_point = distance_from_potential_query;

            this->pushDownIndefinitely(0, false);

            return true;
        }

        return false;
    }


/*
 * Public member function that adds a point to the priority queue of nearest neighbors if the priority queue is below
 * capacity or if the point is closer than an existing neighbor. If the point is closer than an existing neighbor AND
 * the priority queue is at capacity, removes the furthest neighbor before adding the point.
 */
    bool registerAsNeighborIfEligible(float* potential_neighbor) {
        double distance_from_query = distanceBetween(this->query_point, potential_neighbor, this->num_dimensions);

        if (this->registerAsNeighborIfNotFull(potential_neighbor, distance_from_query)) { return true; }

        if (this->registerAsNeighborIfCloser(potential_neighbor, distance_from_query)) { return true; }

        delete[] potential_neighbor;

        return false;
    }
};



inline std::size_t KNNQueue::findDescendantOneChild(std::size_t index, bool on_max_level) const {
    std::size_t left_child = 2 * index + 1;

    return index + ((left_child - index) & (0 - ((this->array[left_child] < this->array[index]) ^ on_max_level)));
}


inline std::size_t KNNQueue::findDescendantBothChildrenNoGrandchildren(std::size_t index, bool on_max_level) const {
    std::size_t left_child = 2 * index + 1;
    std::size_t most_extreme_child = left_child + ((this->array[left_child + 1] < this->array[left_child]) ^ on_max_level);

    return index + ((most_extreme_child - index) & (0 - ((this->array[most_extreme_child] < this->array[index]) ^ on_max_level)));
}


inline std::size_t KNNQueue::findDescendantBothChildrenSomeGrandchildren(std::size_t index, bool on_max_level) const {
    std::size_t left_child = 2 * index + 1;
    std::size_t leftmost_grandchild = 4 * index + 3;

    std::size_t descendant = left_child + ((this->array[left_child + 1] < this->array[left_child]) ^ on_max_level);

    for (std::size_t i = leftmost_grandchild; i < this->current_size; ++i) {
        descendant = ((this->array[i] < this->array[descendant]) ^ on_max_level) ? i : descendant;
    }

    return ((this->array[index] < this->array[descendant]) ^ on_max_level) ? index : descendant;
}


inline std::size_t KNNQueue::findDescendantFullFamily(std::size_t index, bool on_max_level) const {
    std::size_t left_child = 2 * index + 1;
    std::size_t leftmost_grandchild = 4 * index + 3;

    std::size_t more_extreme_child = left_child + ((this->array[left_child + 1] < this->array[left_child]) ^ on_max_level);
    std::size_t more_extreme_left_grandchild = leftmost_grandchild + ((this->array[leftmost_grandchild + 1] < this->array[leftmost_grandchild]) ^ on_max_level);
    std::size_t more_extreme_right_grandchild = leftmost_grandchild + 2 + ((this->array[leftmost_grandchild + 3] < this->array[leftmost_grandchild + 2]) ^ on_max_level);
    std::size_t most_extreme_of_parent_and_children = index + ((more_extreme_child - index) & (0 - ((this->array[more_extreme_child] < this->array[index]) ^ on_max_level)));
    std::size_t most_extreme_grandchild =  more_extreme_left_grandchild + ((more_extreme_right_grandchild - more_extreme_left_grandchild) & (0 - ((this->array[more_extreme_right_grandchild] < this->array[more_extreme_left_grandchild]) ^ on_max_level)));

    return most_extreme_of_parent_and_children + ((most_extreme_grandchild - most_extreme_of_parent_and_children) & (0 - ((this->array[most_extreme_grandchild] < this->array[most_extreme_of_parent_and_children]) ^ on_max_level)));
}


inline void KNNQueue::pushDownOneChild(std::size_t index, bool on_max_level) {
    std::swap(this->array[index], this->array[this->findDescendantOneChild(index, on_max_level)]);
}


inline void KNNQueue::pushDownBothChildrenNoGrandchildren(std::size_t index, bool on_max_level) {
    std::swap(this->array[index], this->array[this->findDescendantBothChildrenNoGrandchildren(index, on_max_level)]);
}





inline void KNNQueue::validate() {
    if (!this->full()) {
        this->makeHeap();
    }
}