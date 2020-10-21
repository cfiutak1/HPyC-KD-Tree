#include <random>
#include <algorithm>
#include <chrono>
#include <cassert>

#include "quickselect/median_of_ninthers.h"
#include "adaptive_blockquickselect_colrow/adaptive_blockquickselect.hpp"





float* generate1DFloatArray(long num_rows, long num_cols) {
    float* array = new float[num_rows * num_cols];
//    printf("%s:%d array size = %lu\n", __FILE__, __LINE__, num_rows * num_cols);

    for (long i = num_rows - 1; i >= 0; --i) {
        for (long j = 0; j < num_cols; ++j) {
            array[(i * num_cols) + j] = rand() % 20;
        }
    }

    return array;
}


float** generate2DimensionalFloatArray(long num_rows, long num_cols) {
    float** array = new float*[num_rows];

    for (long i = 0; i < num_rows; ++i) {
        array[i] = new float[num_cols];
//        std::generate(array[i], array[i] + num_cols, std::rand);

        for (int j = 0; j < num_cols; ++j) {
            array[i][j] = std::rand() % 20;
        }
    }

    return array;
}


void correctlyPartitionedColRow(float** nodes, std::size_t pivot_index, std::size_t num_points, uint64_t depth) {
    auto temp = 0;

    for (auto i = 0; i < pivot_index; ++i) {
        assert (nodes[depth][i] <= nodes[depth][pivot_index]);
    }

    for (auto i = pivot_index + 1; i < num_points; ++i) {
        assert (nodes[depth][i] >= nodes[depth][pivot_index]);
    }
}


void correctlyPartitioned(float* begin, float* pivot, float* end, uint64_t num_dimensions, uint64_t depth) {
    float* temp = begin;

    while (temp != pivot) {
//        printf("%s:%d %f and %f\n", __FILE__, __LINE__, *(temp + depth) , *(pivot + depth));
        assert (*(temp + depth) <= *(pivot + depth));
        temp += num_dimensions;
    }

    temp += num_dimensions;

    while (temp != end) {
        assert (*(temp + depth) >= *(pivot + depth));
        temp += num_dimensions;
    }
}


int main() {
    std::srand(42);
    int num_trials = 1000000;
//    long num_elements = 100;
    long num_dimensions = 5;
    unsigned int depth = 1;
    double diff = 0.0;

    for (long num_elements = 1 << 3; num_elements < (1 << 25); num_elements <<= 1) {
        for (int i = 0; i < num_trials; ++i) {
            float** nodes = generate2DimensionalFloatArray(num_dimensions, num_elements);
//
//            printf("%s:%d ", __FILE__, __LINE__);
//
//            for (long j = 0; j < num_elements; ++j) {
////                printf("[");
////                for (long k = 0; k < num_dimensions; k++) {
//                    printf("%f ", nodes[depth][j]);
////                }
////                printf("], ");
//            }
//
//            printf("\n");

            auto build_start = std::chrono::steady_clock::now();

            AdaptiveCacheAwareBlockquickselect<> acabqs(nodes, num_dimensions, depth);

            acabqs.nth_element(0, num_elements, num_elements / 2);

            auto build_end = std::chrono::steady_clock::now();
            std::chrono::duration<double> build_diff = (build_end - build_start);
            diff += build_diff.count();

//            printf("%s:%d ", __FILE__, __LINE__);
//
//            for (long j = 0; j < num_elements; ++j) {
//                printf("[");
//                for (long k = 0; k < num_dimensions; k++) {
//                    printf("%f ", nodes[(j * num_dimensions) + k]);
//                }
//                printf("], ");
//            }
//
//
//            printf("\n");

            correctlyPartitionedColRow(nodes, num_elements / 2, num_elements, depth);
//            printf("%s:%d %d\n", __FILE__, __LINE__, i);



//        printf("%lu\n", nodes);
//        printf("%lu\n", &(nodes[0][0]));

        for (int j = 0; j < num_dimensions; ++j) {
            delete nodes[j];
        }

            delete[] nodes;
        }

        printf("%lu, %.10f\n", num_elements, diff / num_trials);
    }

}