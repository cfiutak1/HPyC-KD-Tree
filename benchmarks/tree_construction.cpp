#include "../benchmark_utils/array_generation.hpp"
#include "../benchmark_utils/time_macros.hpp"

#include "../src/kdtree/KDTree.hpp"

#include <cstdio>


template<typename ItemT, int(*GeneratorT)()>
float runConstructionBenchmark(std::size_t num_points, std::size_t num_dimensions) {
    TIMING_INIT_VARS()

    ItemT** data = generateMatrix<ItemT, GeneratorT>(num_dimensions, num_points);

    TIMING_START()
    hpyc::KDTree<ItemT> tree(data, num_points, num_dimensions);
    TIMING_END()

    TIMING_CALCULATE_DIFF()

    return timing_recorded;
}


template <typename ItemT, int(*GeneratorT)()>
float runConstructionBenchmarks(std::size_t num_points, std::size_t num_dimensions, std::size_t runs) {
    TIMING_INIT_VARS()

    float* timings = new float[runs];

    for (std::size_t i = 0; i < runs; ++i) {
//        printf("%s:%d %lu\n", __FILE__, __LINE__, i);
        ItemT** data = generateMatrix<ItemT, GeneratorT>(num_dimensions, num_points);

        TIMING_START()
        hpyc::KDTree<ItemT> tree(data, num_points, num_dimensions);
        TIMING_END()

        TIMING_CALCULATE_DIFF()

        timings[i] = timing_recorded;

        for (std::size_t j = 0; j < num_dimensions; j++) {
            delete[] data[j];
        }

//        printf("%s:%d\n", __FILE__, __LINE__);

        delete[] data;
    }

    float min_time = *std::min(timings, timings + runs);

    delete[] timings;

    return min_time;
}



int main() {
    srand(42);
    float time = runConstructionBenchmarks<float, std::rand>(1 << 15, 7, 1000);

    printf("%f\n", time);
}