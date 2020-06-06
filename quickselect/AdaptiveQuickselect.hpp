#ifndef ADAPTIVEQUICKSELECT_HPP
#define ADAPTIVEQUICKSELECT_HPP

#include <vector>
#include <functional>
#include "Partition.hpp"
//#include "MedianOfNinthers.hpp"
#include "Comparators.hpp"


class AdaptiveQuickselect {
private:
    typedef typename std::vector<float*>::iterator iter_t;
    // 2^17 - Clang doesn't like pow() in constexprs
    static constexpr const long SAMPLING_THRESHOLD = 1 << 17;

    unsigned long dimension = 0;

    Partition partitioner;

public:
    AdaptiveQuickselect(unsigned long dimension_in):
        dimension(dimension_in),
        partitioner(dimension_in)
	{}


    iter_t adaptiveQuickselect(iter_t begin_iter, iter_t end_iter, long nth_item);

    template <bool (*comp)(const float*, const float*, const unsigned long&)>
    void medianOfExtrema(
        const iter_t& subarray_begin,
        const iter_t& subarray_end,
        const iter_t& extrema_chunk_begin,
        const iter_t& extrema_chunk_end)
    {
        long extrema_to_compute = std::distance(extrema_chunk_begin, extrema_chunk_end);
        long chunk_size = std::distance(subarray_begin, subarray_end) / extrema_to_compute;

        for (long i = 0; i < extrema_to_compute; ++i) {
            iter_t chunk_extreme = subarray_begin + (i * chunk_size);

            for (iter_t traverser = chunk_extreme + 1; traverser != (subarray_begin + ((i + 1) * chunk_size)); ++traverser) {
                if (comp(*traverser, *chunk_extreme, this->dimension)) chunk_extreme = traverser;
            }

            if (comp(*chunk_extreme, *(extrema_chunk_begin + i), this->dimension)) {
                std::swap(*chunk_extreme, *(extrema_chunk_begin + i));
            }
        }
    }


    iter_t medianOfMinima(const iter_t& array_begin, const iter_t& array_end, const long& partition_index);

    iter_t medianOfMaxima(const iter_t& array_begin, const iter_t& array_end, const long& partition_index);

    iter_t medianOfNinthers(const iter_t& array_begin, const iter_t& array_end);

    inline iter_t median3(iter_t iter1, iter_t iter2, iter_t iter3) {
        if (!(comp_lte(*iter1, *iter2, this->dimension) ^ comp_gt(*iter1, *iter3, this->dimension))) { return iter1; }
        else if (!(comp_lt(*iter2, *iter1, this->dimension) ^ comp_lt(*iter2, *iter3, this->dimension))) { return iter2; }
        return iter3;
    }

    inline iter_t ninther(const iter_t& array_l_begin, const iter_t& array_m_begin, const iter_t& array_r_begin, const long& subsample_size) {
        iter_t nin = this->median3(
            this->median3(array_l_begin, array_m_begin, array_r_begin),
            this->median3(array_l_begin + 1, array_m_begin + (subsample_size / 3), array_r_begin + 1),
            this->median3(array_l_begin + 2, array_m_begin + (2 * (subsample_size / 3)), array_r_begin + 2)
        );

        std::swap(*nin, *(array_m_begin + (subsample_size / 3)));

        return nin;
    }
};

#endif
