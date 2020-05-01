#ifndef PARTITION_HPP
#define PARTITION_HPP


#include "../kdtree/Point.hpp"
#include <vector>



class Partition {
private:
    typedef std::vector<Point*>::iterator iter_t;

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

    inline void comp_and_swap(std::vector<Point*>::iterator& iter1, std::vector<Point*>::iterator& iter2) {
        std::vector<Point*>::iterator item = iter1;
        iter1 = (comp_lt(*iter2, *item, this->dimension)) ? iter2 : iter1;
        iter2 = (comp_lt(*iter2, *item, this->dimension)) ? item : iter2;
    }

    inline std::vector<Point*>::iterator sort3(std::vector<Point*>::iterator iter1, std::vector<Point*>::iterator iter2, std::vector<Point*>::iterator iter3) {
        comp_and_swap(iter1, iter2);
        comp_and_swap(iter2, iter3);
        comp_and_swap(iter1, iter2);

        return iter2;
    }

public:
    Partition(unsigned long dimension_in): dimension(dimension_in) {}


    iter_t hoarePartition(const iter_t& begin, const iter_t& end, const unsigned long& partition_index) {
        // if (end - begin == 3) {
        //     this->sort3(begin, begin + 1, end);
        //
        //     return begin + 1;
        // }


        std::swap(*begin, *(begin + partition_index));

        iter_t forward = begin;
        iter_t reverse = end - 1;

        // {
        //     auto temp = begin;
        //
        //     printf("%s:%d\n", __FILE__, __LINE__);
        //     while (temp != end) {
        //         printf("\t");
        //         (*temp)->printCoordinates();
        //
        //         ++temp;
        //     }
        //
        // }

        while (forward != reverse) {

            do { ++forward; } while ((forward != reverse) && (comp_lt(*forward, *begin, this->dimension)));

            if (forward == reverse) break;

            do { --reverse; } while ((forward != reverse) && comp_gt(*reverse, *begin, this->dimension));

            if (forward == reverse) {
                std::swap(*begin, *(reverse - 1));

                return reverse - 1;
            }

            std::swap(*forward, *reverse);
        }

        // if (forward == end - 1 && forward == reverse) {
        //     return forward - 1;
        // }

        if (reverse == end - 1) {
            if (comp_lt(*reverse, *begin, this->dimension)) {
                std::swap(*begin, *reverse);

                return reverse;
            }



        }

        std::swap(*begin, *(reverse - 1));

        // {
        //     auto temp = begin;
        //
        //     printf("%s:%d\n", __FILE__, __LINE__);
        //     while (temp != end) {
        //         printf("\t");
        //         (*temp)->printCoordinates();
        //         ++temp;
        //     }
        // }

        return reverse - 1;
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
        Point* pivot_value = *pivot;

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
