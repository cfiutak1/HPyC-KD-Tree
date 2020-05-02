#ifndef PARTITION_HPP
#define PARTITION_HPP

#include "../kdtree/KDNode.hpp"
#include <vector>
//#include <cassert>


class Partition {
private:
    typedef std::vector<KDNode*>::iterator iter_t;

    unsigned long dimension = 0;

    inline void movePivotRight(iter_t& pivot, const iter_t& right_subarray_traverser) const {
        iter_t original_pivot = pivot;

        while (comp_lte(*(pivot + 1), *original_pivot, this->dimension) && (pivot + 1) != right_subarray_traverser) ++pivot;

        std::swap(*pivot, *original_pivot);
    }

    inline void movePivotLeft(iter_t& pivot, const iter_t& left_subarray_traverser) const {
        iter_t original_pivot = pivot;

        while (((pivot - 1) != left_subarray_traverser) && comp_gte(*(pivot - 1), *original_pivot, this->dimension)) --pivot;

        std::swap(*pivot, *original_pivot);
    }

public:
    //Partition() = default;
    Partition(unsigned long dimension_in): dimension(dimension_in) {
    	//printf("%s:%d %lu\n", __FILE__, __LINE__, dimension_in);
    }


    iter_t hoarePartition(const iter_t& begin, const iter_t& end, const unsigned long& partition_index) {
//	    printf("%s:%d len=%lu pi=%lu\n", __FILE__, __LINE__, end - begin, partition_index);
        std::swap(*begin, *(begin + partition_index));

        iter_t forward = begin + 1;
        iter_t reverse = end - 1;

        while (true) {

            while (true) {
                if (std::distance(begin, forward) > std::distance(begin, reverse)) goto end_loop;
//		printf("%s:%d dim=%lu\n", __FILE__, __LINE__, this->dimension);
                if (comp_gte( *forward, *begin,this->dimension)) break;
                ++forward;
            }

            while (comp_lt(*begin, *reverse, this->dimension)) --reverse;

            if (std::distance(begin, forward) >= std::distance(begin, reverse)) break;



            std::swap(*forward, *reverse);

            ++forward;
            --reverse;
        }

        end_loop:
            std::swap(*begin, *(forward- 1));

            return forward - 1;
    }


    iter_t expandPartition(
        const iter_t& array_begin,
        const iter_t& left_subarray_end,
        iter_t& pivot,
        const iter_t& right_subarray_begin,
        const iter_t& array_end)
    {
        iter_t left_subarray_traverser = array_begin;
        iter_t right_subarray_traverser = array_end - 1;

        // this being a pointer may cause problems
        KDNode* pivot_value = *pivot;

        while (left_subarray_traverser != left_subarray_end && right_subarray_traverser != (right_subarray_begin)) {
            if (comp_gt(*left_subarray_traverser, pivot_value, this->dimension) && comp_lt(*right_subarray_traverser, pivot_value, this->dimension)) {
                std::swap(*left_subarray_traverser, *right_subarray_traverser);
                ++left_subarray_traverser;
                --right_subarray_traverser;
            }

            else {
                if (comp_lte(*left_subarray_traverser, pivot_value, this->dimension)) ++left_subarray_traverser;

                if (comp_gte(*right_subarray_traverser, pivot_value, this->dimension)) --right_subarray_traverser;
            }
        }

        if (left_subarray_traverser == left_subarray_end && right_subarray_traverser == right_subarray_begin) {
            return pivot;
        }

        if (left_subarray_traverser == left_subarray_end) {
            do {
                movePivotRight(pivot, right_subarray_traverser);

                while (comp_gte(*right_subarray_traverser, pivot_value, this->dimension) && (right_subarray_traverser != right_subarray_begin) && (right_subarray_traverser - 1 != pivot)) {
                    --right_subarray_traverser;
                }

                if (right_subarray_traverser == right_subarray_begin || (right_subarray_traverser - 1 == pivot)) break;

                if (pivot == array_end - 1) break;

                std::swap(*right_subarray_traverser, *(pivot + 1));
                std::swap(*pivot, *(pivot + 1));

                ++pivot;
            } while (right_subarray_traverser != right_subarray_begin);

            if (comp_gt(*pivot, *(pivot + 1), this->dimension)) {
                std::swap(*pivot, *(pivot + 1));
                ++pivot;
            }

            return pivot;
        }

        else {
            do {
                movePivotLeft(pivot, left_subarray_traverser);

                while (comp_lte(*left_subarray_traverser, pivot_value, this->dimension) && (left_subarray_traverser != left_subarray_end) && (left_subarray_traverser + 1 != pivot)) {
                    ++left_subarray_traverser;
                }

                if (left_subarray_traverser == left_subarray_end || (left_subarray_traverser + 1 == pivot)) break;
                if (pivot == array_begin) break;

                std::swap(*left_subarray_traverser, *(pivot - 1));
                std::swap(*pivot, *(pivot - 1));

                --pivot;
            } while (left_subarray_traverser != left_subarray_end);

            if (comp_lt(*pivot, *(pivot - 1), this->dimension)) {
                std::swap(*pivot, *(pivot - 1));
                --pivot;
            }

            return pivot;
        }
    }
};

#endif
