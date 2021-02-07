#include "src/filedata/TrainingFileData.hpp"
#include "src/filedata/QueryFileData.hpp"
#include "src/fileprocessor/QueryFileProcessor.hpp"
#include "src/fileprocessor/TrainingFileProcessor.hpp"
#include "src/filewriter/ResultsFileWriter.hpp"
#include "src/kdtree/KDTree.hpp"
#include "src/parallel_kdtree/ParallelKDTree.hpp"

#include <cstdlib>
#include <cstdio>
#include <chrono>
#include <sys/stat.h>


inline bool file_exists (const std::string& name) {
    // https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}


void cleanup(TrainingFileData* training_file_data, QueryFileData* query_file_data, float* training_points, float** query_points) {
    delete[] training_points;

    for (std::size_t i = 0; i < query_file_data->num_points; ++i) {
        delete query_points[i];
    }

    delete[] query_points;

    delete training_file_data;
    delete query_file_data;
}


void runNearestNeighbors(TrainingFileProcessor& training_file_processor, QueryFileProcessor& query_file_processor, std::string result_file_name) {
    TrainingFileData* training_file_data = training_file_processor.readTrainingFileHeader();

    alignas(32) float* training_points = training_file_processor.readPoints1D();

    hpyc::KDTree<float> tree(training_points, training_file_data->num_points, training_file_data->num_dimensions);

    QueryFileData* query_file_data = query_file_processor.readQueryFileHeader();
    alignas(32) float** query_point = query_file_processor.readPointsRowCol();

    ResultsFileWriter writer(result_file_name, training_file_data, query_file_data);
    writer.writeFileHeader();


    auto query_start = std::chrono::steady_clock::now();

    std::size_t* indices = new std::size_t[query_file_data->num_neighbors];
    double* distances = new double[query_file_data->num_neighbors];

    tree.nearestNeighborsSearch(query_point[0], query_file_data->num_neighbors, indices, distances);

    auto query_end = std::chrono::steady_clock::now();
    std::chrono::duration<double> query_diff = (query_end - query_start);
    printf("query %f\n", query_diff.count());

    for (std::size_t i = 0; i < query_file_data->num_dimensions; ++i) {
        printf("%f ", distances[i]);
    }

    printf("\n");

    cleanup(training_file_data, query_file_data, training_points, query_point);
}


int main(int argc, char** argv) {
    if (argc != 4) {
        printf("Error: Invalid number of arguments (got %d, expected 44)\n", argc);

        return 1;
    }

    std::string training_file_name = argv[1];
    if (!file_exists(training_file_name)) {
        printf("Error: Could not find training file <%s>\n", training_file_name.c_str());

        return 1;
    }

    std::string query_file_name = argv[2];
    if (!file_exists(query_file_name)) {
        printf("Error: Could not find query file <%s>\n", query_file_name.c_str());

        return 1;
    }

    std::string result_file_name = argv[3];

    TrainingFileProcessor training_file_processor(training_file_name);
    QueryFileProcessor query_file_processor(query_file_name);

    runSingleThreadedOneQuery(training_file_processor, query_file_processor, result_file_name);
}
