#include "Neighbor.hpp"

#include <cstdint>
#include <algorithm>
#include <cstdio>
#include <cassert>

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

inline bool is_new_item_min(uint64_t length)
{
    return (highestSetBitPosition(length) & 1) == 0;
}

inline bool is_min_item(uint64_t index)
{
    return is_new_item_min(index + 1);
}


inline std::size_t first_child_index(std::size_t i) { return 2 * i + 1; }


template<typename It>
bool is_minmax_heap(It begin, It end)
{
    uint64_t length = static_cast<uint64_t>(end - begin);
    auto test_index = [](uint64_t index, auto compare_index)
    {
        uint64_t first_child = first_child_index(index);
        uint64_t second_child = first_child + 1;
        uint64_t first_grandchild = first_child_index(first_child);
        uint64_t second_grandchild = first_grandchild + 1;
        uint64_t third_grandchild = first_child_index(second_child);
        uint64_t fourth_grandchild = third_grandchild + 1;
        return compare_index(first_child) && compare_index(second_child)
               && compare_index(first_grandchild) && compare_index(second_grandchild)
               && compare_index(third_grandchild) && compare_index(fourth_grandchild);
    };
    for (uint64_t i = 0; i < length; ++i)
    {
        if (is_min_item(i))
        {
            auto compare_one = [&](uint64_t child)
            {
                return child >= length || !(begin[child] < begin[i]);
            };
            if (!test_index(i, compare_one))
                return false;
        }
        else
        {
            auto compare_one = [&](uint64_t child)
            {
                return child >= length || !(begin[i] < begin[child]);
            };
            if (!test_index(i, compare_one))
                return false;
        }
    }
    return true;
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

        if (descendant == 0) return 0; // TODO i just spammed this to hopefully make things work

        // Swaps the current item at a[descendant] with its parent, if necessary
        std::size_t descendant_parent = getParentIndex(descendant);
//        printf("%s:%d index=%lu, descendant=%lu, descendant_parent=%lu, this->current_size=%lu\n", __FILE__, __LINE__, index, descendant, descendant_parent, this->current_size);
        bool swap_desc = ((this->array[descendant_parent] < this->array[descendant]) ^ on_max_level) && (descendant > (2 * index + 2));

        std::swap(
            this->array[descendant],
            this->array[descendant - ((descendant - descendant_parent) & (0u - swap_desc))]
        );

        return descendant;
    }


    inline void pushDownIndefinitely(std::size_t index, std::size_t first_index_with_no_grandchildren, std::size_t first_index_with_no_children, bool on_max_level) {
//        printf("%s:%d index=%lu, first_index_with_no_grandchildren=%lu, first_index_with_no_children=%lu, on_max_level=%d, current_size = %lu\n", __FILE__, __LINE__, index, first_index_with_no_grandchildren, first_index_with_no_children, on_max_level, current_size);

        while (index < first_index_with_no_grandchildren && (4 * index + 6) < this->current_size) {
            std::size_t new_index = pushDownTwoLevels<&KNNQueue::findDescendantFullFamily>(index, on_max_level);

            on_max_level ^= ((new_index == 2 * index + 1) || (new_index == 2 * index + 2));

            if (new_index == index) return;
            index = new_index;
        }


//        printf("%s:%d index=%lu current_size=%lu\n", __FILE__, __LINE__, index, this->current_size);

        if (2 * index + 1 >= this->current_size) return;

        else if (4 * index + 6 < this->current_size) {
            pushDownTwoLevels<&KNNQueue::findDescendantFullFamily>(index, on_max_level);
        }

        else if (4 * index + 3 < this->current_size){
            pushDownTwoLevels<&KNNQueue::findDescendantBothChildrenSomeGrandchildren>(index, on_max_level);
        }

        else if (index < first_index_with_no_children) {
            this->pushDownBothChildrenNoGrandchildren(index, on_max_level);
        }

        else if ((index == first_index_with_no_children) && (this->current_size % 2 == 0)) {
            this->pushDownOneChild(index, on_max_level);
        }

    }

    inline bool closerThanFarthestNeighbor(const double& p) const {
        return p < this->array[0].distance_from_queried_point;
    }

    friend class ThreadSafeKNNQueue;


public:
    KNNQueue() {
//        printf("%s:%d default constructor\n", __FILE__, __LINE__);
    }

    KNNQueue(const float* query_point_in, const uint64_t& num_neighbors_in, const uint64_t& num_dimensions_in):
        query_point(query_point_in),
        capacity(num_neighbors_in),
        num_dimensions(num_dimensions_in)
    {
//        printf("%s:%d proper constructor\n", __FILE__, __LINE__);
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

    void printHeap() {
        for (std::size_t i = 0; i < this->current_size; ++i) {
            printf("x_coor = %f, dist = %f \n", this->array[i].point[0], this->array[i].distance_from_queried_point);
        }

        printf("\n");
    }

    // TODO need to test pop functions on small inputs
    // TODO move point deletion here
    inline void popMin() {
//        this->printHeap();
//        printf("%s:%d popMin on size %lu\n", __FILE__, __LINE__, this->current_size);
        std::swap(this->array[0], this->array[this->current_size - 1]);
        --this->current_size;

        std::size_t first_index_without_all_grandchildren = (this->current_size / 4) - (4 * ((this->current_size / 4) - 1) + 6 >= this->current_size);
        std::size_t first_index_without_all_children = (this->current_size / 2) - (this->current_size % 2 == 0);
        this->pushDownIndefinitely(0, first_index_without_all_grandchildren, first_index_without_all_children, false);
//        this->printHeap();

//        assert (is_minmax_heap(this->array, this->array + this->current_size));
    }

    inline void popMax() {
//        printf("%s:%d popMax on size %lu\n", __FILE__, __LINE__, this->current_size);
//        this->printHeap();

        if (this->current_size < 3) {
            --this->current_size;
            return;
        }

        std::size_t largest_item = 1 + (this->array[1] < this->array[2]);

        std::swap(this->array[largest_item], this->array[this->current_size - 1]);

//        this->printHeap();

        --this->current_size;

        std::size_t first_index_without_all_grandchildren = (this->current_size / 4) - (4 * ((this->current_size / 4) - 1) + 6 >= this->current_size);
        std::size_t first_index_without_all_children = (this->current_size / 2) - (this->current_size % 2 == 0);
        this->pushDownIndefinitely(largest_item, first_index_without_all_grandchildren, first_index_without_all_children, true);

//        this->printHeap();

//        printf("%s:%d\n", __FILE__, __LINE__, this->current_size);
//        assert (is_minmax_heap(this->array, this->array + this->current_size));

//        this->printHeap();
    }


    Neighbor& getMin() const {
        return this->array[0];
    }

    Neighbor& getMax() const {
        if (this->current_size < 3) {
            return this->array[this->current_size - 1];
        }


//        printf("%s:%d a[1]=%f, a[2]=%f, max=%f\n", __FILE__, __LINE__, this->array[1].distance_from_queried_point, this->array[2].distance_from_queried_point, this->array[1 + (this->array[1] < this->array[2])].distance_from_queried_point);

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
//            printf("%s:%d updated %lu-th neighbor\n", __FILE__, __LINE__, this->current_size);

//            this->array[this->current_size] = {potential_neighbor, distance_from_query};

            ++this->current_size;

//            printf("%s:%d\n", __FILE__, __LINE__);
            if (this->current_size == this->capacity) {
//                this->printHeap();
                this->makeHeap();
//                assert (is_minmax_heap(this->array, this->array + this->current_size));
//                printf("%s:%d\n", __FILE__, __LINE__);
//                this->printHeap();
//                printf("%s:%d\n", __FILE__, __LINE__);
            }
//            printf("%s:%d\n", __FILE__, __LINE__);

            return true;
        }

        return false;
    }

    inline bool registerAsNeighborIfCloser(float* potential_neighbor, double distance_from_potential_query) {
        // If the priority queue is at capacity and the potential neighbor is closer to the query point than the current
        // furthest neighbor, remove the furthest neighbor from the priority queue and push the potential neighbor.
        if (this->closerThanFarthestNeighbor(distance_from_potential_query)) {
//            printf("%s:%d\n", __FILE__, __LINE__);

//            std::size_t replace_index;

//            if (this->current_size < 3) {
//                replace_index = this->current_size - 1;
//            }
//
//            else {
//                replace_index = 1 + (this->array[1] < this->array[2]);
//            }

//            printf("%s:%d current_size=%lu\n", __FILE__, __LINE__, this->current_size);

//            printf("%s:%d\n", __FILE__, __LINE__);
//            this->printHeap();
//            printf("%s:%d\n", __FILE__, __LINE__);
            delete[] this->array[0].point;
            this->array[0].point = potential_neighbor;
            this->array[0].distance_from_queried_point = distance_from_potential_query;

            std::size_t first_index_without_all_grandchildren = (this->current_size / 4) - (4 * ((this->current_size / 4) - 1) + 6 >= this->current_size);
            std::size_t first_index_without_all_children = (this->current_size / 2) - (this->current_size % 2 == 0);
            this->pushDownIndefinitely(0, first_index_without_all_grandchildren, first_index_without_all_children, false);

//            this->pushDownIndefinitely(0, false);
//            printf("%s:%d\n", __FILE__, __LINE__);
//            this->printHeap();
//            printf("%s:%d\n", __FILE__, __LINE__);
//            printf("%s:%d\n", __FILE__, __LINE__);
//            assert (is_minmax_heap(this->array, this->array + this->current_size));

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
//    printf("%s:%d index=%lu size=%lu\n", __FILE__, __LINE__, index, this->current_size);
    assert (2 * index + 1 < this->current_size);
    std::size_t left_child = 2 * index + 1;


    return index + ((left_child - index) & (0 - ((this->array[left_child] < this->array[index]) ^ on_max_level)));
}


inline std::size_t KNNQueue::findDescendantBothChildrenNoGrandchildren(std::size_t index, bool on_max_level) const {
//    printf("%s:%d index=%lu\n", __FILE__, __LINE__, index);
    assert (2 * index + 2 < this->current_size);
    std::size_t left_child = 2 * index + 1;
    std::size_t most_extreme_child = left_child + ((this->array[left_child + 1] < this->array[left_child]) ^ on_max_level);

    return index + ((most_extreme_child - index) & (0 - ((this->array[most_extreme_child] < this->array[index]) ^ on_max_level)));
}


inline std::size_t KNNQueue::findDescendantBothChildrenSomeGrandchildren(std::size_t index, bool on_max_level) const {
    std::size_t left_child = 2 * index + 1;
    std::size_t leftmost_grandchild = 4 * index + 3;

    assert (leftmost_grandchild < this->current_size);

    std::size_t descendant = left_child + ((this->array[left_child + 1] < this->array[left_child]) ^ on_max_level);

    for (std::size_t i = leftmost_grandchild; i < this->current_size; ++i) {
        descendant = ((this->array[i] < this->array[descendant]) ^ on_max_level) ? i : descendant;
    }

    return ((this->array[index] < this->array[descendant]) ^ on_max_level) ? index : descendant;
}


inline std::size_t KNNQueue::findDescendantFullFamily(std::size_t index, bool on_max_level) const {
//    printf("%s:%d index=%lu size=%lu\n", __FILE__, __LINE__, index, this->current_size);
    assert(4 * index + 6 < this->current_size);
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
    assert(2 * index + 1 < this->current_size);
//    printf("%s:%d swapping a[%lu]=%f and a[%lu]=%f\n", __FILE__, __LINE__, index, this->array[index].distance_from_queried_point, this->findDescendantOneChild(index, on_max_level), this->array[this->findDescendantOneChild(index, on_max_level)].distance_from_queried_point);
    std::swap(this->array[index], this->array[this->findDescendantOneChild(index, on_max_level)]);
}


inline void KNNQueue::pushDownBothChildrenNoGrandchildren(std::size_t index, bool on_max_level) {
    assert(2 * index + 2 < this->current_size);
//    printf("%s:%d index=%lu\n", __FILE__, __LINE__, index);
//    printf("%s:%d swapping a[%lu]=%f and a[%lu]=%f\n", __FILE__, __LINE__, index, this->array[index].distance_from_queried_point, this->findDescendantBothChildrenNoGrandchildren(index, on_max_level), this->array[this->findDescendantBothChildrenNoGrandchildren(index, on_max_level)].distance_from_queried_point);
    std::swap(this->array[index], this->array[this->findDescendantBothChildrenNoGrandchildren(index, on_max_level)]);
}





inline void KNNQueue::validate() {
    if (!this->full()) {
        this->makeHeap();
    }
}