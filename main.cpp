#include "filedata/TrainingFileData.hpp"
#include "filedata/QueryFileData.hpp"
#include "fileprocessor/QueryFileProcessor.hpp"
#include "fileprocessor/TrainingFileProcessor.hpp"
#include "filewriter/ResultsFileWriter.hpp"
#include "kdtree/KDTree.hpp"
#include "kdtree/KNNQueue.hpp"
//#include "parallel_kdtree/ParallelKDTree.hpp"

#include <cstdlib>
#include <cstdio>
#include <chrono>
#include <sys/stat.h>


inline bool file_exists (const std::string& name) {
    // https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}


template <typename TreeType>
void cleanup(TrainingFileData* training_file_data, QueryFileData* query_file_data, float** training_points, float** query_points, TreeType* tree) {
    for (uint64_t i = 0; i < training_file_data->num_dimensions; ++i) {
        delete[] training_points[i];
    }

    for (uint64_t i = 0; i < query_file_data->num_points; ++i) {
        delete[] query_points[i];
    }


    delete[] training_points;
    delete[] query_points;

    delete tree;
    delete training_file_data;
    delete query_file_data;
}


void runSingleThreaded(TrainingFileProcessor& training_file_processor, QueryFileProcessor& query_file_processor, std::string result_file_name) {
    auto file_read_and_build_start = std::chrono::steady_clock::now();

    TrainingFileData* training_file_data = training_file_processor.readTrainingFileHeader();
    alignas(32) float** training_points = training_file_processor.readPointsColRow();

    auto build_start = std::chrono::steady_clock::now();

    KDTree* tree = new KDTree(training_points, training_file_data->num_points, training_file_data->num_dimensions);

    auto file_read_and_build_end = std::chrono::steady_clock::now();
    auto build_end = std::chrono::steady_clock::now();
    std::chrono::duration<double> file_read_and_build_diff = (file_read_and_build_end - file_read_and_build_start);
    std::chrono::duration<double> build_diff = (build_end - build_start);
    printf("file_read_and_build %f\n", file_read_and_build_diff.count());
    printf("build %f\n", build_diff.count());


    QueryFileData* query_file_data = query_file_processor.readQueryFileHeader();
    alignas(32) float** query_points = query_file_processor.readPoints();

    auto query_and_file_out_start = std::chrono::steady_clock::now();
    ResultsFileWriter writer(result_file_name, training_file_data, query_file_data);
    writer.writeFileHeader();

    PointAllocator point_allocator(training_file_data->num_dimensions, query_file_data->num_neighbors);

    for (uint64_t i = 0; i < query_file_data->num_points; ++i) {
        KNNQueue results(query_points[i], query_file_data->num_neighbors, query_file_data->num_dimensions, point_allocator);
        tree->nearestNeighborsSearch(query_points[i], results);
        writer.writeQueryResults(results);
    }

    auto query_and_file_out_end = std::chrono::steady_clock::now();
    std::chrono::duration<double> query_and_file_out_diff = (query_and_file_out_end - query_and_file_out_start);
    printf("query_and_file_out %f\n", query_and_file_out_diff.count());

    cleanup(training_file_data, query_file_data, training_points, query_points, tree);
}


//void runMultiThreaded(TrainingFileProcessor& training_file_processor, QueryFileProcessor& query_file_processor, std::string& result_file_name, int num_threads) {
//    auto file_read_and_build_start = std::chrono::steady_clock::now();
//
//    TrainingFileData* training_file_data = training_file_processor.readTrainingFileHeader();
//    alignas(32) float** training_points = training_file_processor.readPointsColRow();
//
//    auto build_start = std::chrono::steady_clock::now();
//
//    ParallelKDTree* tree = new ParallelKDTree(training_points, training_file_data->num_points, training_file_data->num_dimensions, num_threads);
//
//    auto file_read_and_build_end = std::chrono::steady_clock::now();
//    auto build_end = std::chrono::steady_clock::now();
//    std::chrono::duration<double> file_read_and_build_diff = (file_read_and_build_end - file_read_and_build_start);
//    std::chrono::duration<double> build_diff = (build_end - build_start);
//    printf("file_read_and_build %f\n", file_read_and_build_diff.count());
//    printf("build %f\n", build_diff.count());
//
//    auto query_and_file_out_start = std::chrono::steady_clock::now();
//
//    QueryFileData* query_file_data = query_file_processor.readQueryFileHeader();
//    alignas(32) float** query_points = query_file_processor.readPoints();
//
//    ResultsFileWriter writer(result_file_name, training_file_data, query_file_data);
//    writer.writeFileHeader();
//
//    KNNQueue* results = tree->nearestNeighborsSearches(query_points, query_file_data->num_points, query_file_data->num_neighbors, num_threads + 1);
//
//    for (uint64_t i = 0; i < query_file_data->num_points; ++i) {
//        writer.writeQueryResults(results[i]);
//    }
//
//
////    for (uint64_t i = 0; i < query_file_data->num_points; ++i) {
////        ThreadSafeKNNQueue result = tree->nearestNeighborsSearch(query_points[i], query_file_data->num_neighbors);
////        writer.writeQueryResults(result);
////    }
//
//    auto query_and_file_out_end = std::chrono::steady_clock::now();
//    std::chrono::duration<double> query_and_file_out_diff = (query_and_file_out_end - query_and_file_out_start);
//    printf("query_and_file_out %f\n", query_and_file_out_diff.count());
//
//    cleanup(training_file_data, query_file_data, training_points, query_points, tree);
//}




int main(int argc, char** argv) {
    if (argc != 5) {
        printf("Error: Invalid number of arguments (got %d, expected 5)\n", argc);

        return 1;
    }

    int num_cores = atoi(argv[1]);
    if (num_cores < 1) {
        printf("Error: The program must use at least one core\n");

        return 1;
    }

    std::string training_file_name = argv[2];
    if (!file_exists(training_file_name)) {
        printf("Error: Could not find training file <%s>\n", training_file_name.c_str());

        return 1;
    }

    std::string query_file_name = argv[3];
    if (!file_exists(query_file_name)) {
        printf("Error: Could not find query file <%s>\n", query_file_name.c_str());

        return 1;
    }

    std::string result_file_name = argv[4];
    if (!file_exists(query_file_name)) {
        printf("Error: Could not find query file <%s>\n", query_file_name.c_str());

        return 1;
    }

    unsigned int num_threads = num_cores - 1;

    TrainingFileProcessor training_file_processor(training_file_name);
    QueryFileProcessor query_file_processor(query_file_name);

    if (num_threads == 0) {
        runSingleThreaded(training_file_processor, query_file_processor, result_file_name);
    }

//    else {
//        runMultiThreaded(training_file_processor, query_file_processor, result_file_name, num_threads);
//    }

}
