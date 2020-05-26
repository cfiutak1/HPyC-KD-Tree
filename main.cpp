#include <cstdlib>
#include <cstdio>
#include <map>
#include <sys/stat.h>

#include "filedata/TrainingFileData.hpp"
#include "filedata/QueryFileData.hpp"
#include "fileprocessor/FileProcessor.hpp"
#include "fileprocessor/QueryFileProcessor.hpp"
#include "fileprocessor/TrainingFileProcessor.hpp"
#include "filewriter/ResultsFileWriter.hpp"
#include "kdtree/KDTree.hpp"
#include "kdtree/KNNQueue.hpp"
//#include "taskmanager/Task.hpp"
//#include "taskmanager/TaskManager.hpp"



inline bool file_exists (const std::string& name) {
    // https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}


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

    unsigned int num_threads = (1 * num_cores) - 1;

    auto file_read_and_build_start = std::chrono::steady_clock::now();
    TrainingFileProcessor training_file_processor(training_file_name);
    TrainingFileData* training_file_data = training_file_processor.readTrainingFileHeader();
    std::vector<float*> training_points = training_file_processor.readPoints();


    auto build_start = std::chrono::steady_clock::now();
    KDTree* tree = new KDTree(training_points, training_file_data->num_dimensions);
    tree->buildTree();
    auto file_read_and_build_end = std::chrono::steady_clock::now();
    auto build_end = std::chrono::steady_clock::now();
    std::chrono::duration<double> file_read_and_build_diff = (file_read_and_build_end - file_read_and_build_start);
    std::chrono::duration<double> build_diff = (build_end - build_start);
    printf("file_read_and_build %f\n", file_read_and_build_diff.count());
    printf("build %f\n", build_diff.count());

    auto query_and_file_out_start = std::chrono::steady_clock::now();

    QueryFileProcessor query_file_processor(query_file_name);
    QueryFileData* query_file_data = query_file_processor.readQueryFileHeader();
    std::vector<float*> query_points = query_file_processor.readPoints();

    ResultsFileWriter writer(result_file_name, training_file_data, query_file_data);



    for (unsigned long i = 0; i < query_points.size(); ++i) {
        KNNQueue* results = tree->nearestNeighborsSearch(query_points[i], query_file_data->num_neighbors);

        writer.writeQueryResults(results);
    }


    auto query_and_file_out_end = std::chrono::steady_clock::now();
    std::chrono::duration<double> query_and_file_out_diff = (query_and_file_out_end - query_and_file_out_start);
    printf("query_and_file_out %f\n", query_and_file_out_diff.count());


    delete tree;
//    for (auto p : training_points) delete p;
//    for (auto p : query_points) delete p;

    delete training_file_data;

//    delete query_file_data;
}
