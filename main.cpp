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
#include "singlequerysearcher/KNNSingleQuerySearcher.hpp"
#include "taskmanager/Task.hpp"
#include "taskmanager/TaskManager.hpp"

#include <iostream>


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

    auto tree_start = std::chrono::steady_clock::now();
    TrainingFileProcessor training_file_processor(training_file_name);
    TrainingFileData* training_file_data = training_file_processor.readTrainingFileHeader();
    std::vector<KDNode*> training_points = training_file_processor.readPoints();



    KDTree* tree = new KDTree(num_threads, training_file_data, training_points);
    // unsigned long num_dimensions = training_file_data->num_dimensions;
    // KDTree* tree = new KDTree(num_threads, num_dimensions, training_points);
    tree->buildTreeParallel();
    auto tree_end = std::chrono::steady_clock::now();
    std::chrono::duration<double> tree_diff = (tree_end - tree_start);
    printf("%f\n", tree_diff.count());

    auto query_start = std::chrono::steady_clock::now();

    QueryFileProcessor query_file_processor(query_file_name);
    QueryFileData* query_file_data = query_file_processor.readQueryFileHeader();
    std::vector<KDNode*> query_points = query_file_processor.readPoints();

    ResultsFileWriter writer(result_file_name, training_file_data, query_file_data);

    if (num_threads > 0) {
//        for (unsigned long i = 0; i < query_points.size(); ++i) {
//            KNNSingleQuerySearcher searcher(tree, *(query_points.begin() + i), query_file_data->num_neighbors, num_threads);
//            KNNQueue* results = searcher.nearestNeighborsSearch();
//            writer.writeQueryResults(results);
//        }
         TaskManager<KDNode, KNNQueue> query_task_manager(query_points, num_threads);
         QueryTask task(tree, query_file_data->num_neighbors);

         std::vector<std::vector<KNNQueue*>> results = query_task_manager.completeTasks<QueryTask>(task);
         writer.generateResultsFile(results);
    }

    else {
        for (unsigned long i = 0; i < query_points.size(); ++i) {
            KNNSearcher searcher(tree, query_file_data->num_neighbors, query_points[i]);
            KNNQueue* results = searcher.nearestNeighborsSearch();
            writer.writeQueryResults(results);
        } 
    }

    auto query_end = std::chrono::steady_clock::now();
    std::chrono::duration<double> query_diff = (query_end - query_start);
    printf("%f\n", query_diff.count());


    delete tree;
    //for (auto p : training_points) delete p;
    for (auto p : query_points) delete p;

    delete training_file_data;

    delete query_file_data;
}
