#ifndef RESULTSFILEWRITER_HPP
#define RESULTSFILEWRITER_HPP

#include <fstream>
#include <string>
#include <vector>
#include <string>
#include <map>
#include <stack>


#include "../kdtree/Neighbor.hpp"
#include "../kdtree/KNNQueue.hpp"
#include "../filedata/FileData.hpp"
#include "../filedata/TrainingFileData.hpp"
#include "../filedata/QueryFileData.hpp"


class ResultsFileWriter {
private:
    std::string file_name;
    TrainingFileData* training_file_data;
    QueryFileData* query_file_data;

    std::ofstream results_file;

    uint64_t generateFileID() {
        std::ifstream urandom("/dev/urandom", std::ios::in|std::ios::binary);
        uint64_t rand = 0;

        if (urandom) {
            urandom.read(reinterpret_cast<char*>(&rand), 8);
            urandom.close();
        }

        return rand;
    }

public:
    ResultsFileWriter(
        std::string file_name,
        TrainingFileData* training_file_data,
        QueryFileData* query_file_data
    ):
        file_name(file_name),
        training_file_data(training_file_data),
        query_file_data(query_file_data),
        results_file(file_name, std::ios::out | std::ios::app | std::ios::binary)
    {}

    ~ResultsFileWriter() { this->results_file.close(); }

    void writeFileHeader() {
        this->results_file << "RESULT\0\0";
        this->results_file << this->training_file_data->file_id;
        this->results_file << this->query_file_data->file_id;
        this->results_file << this->generateFileID();
        this->results_file << this->query_file_data->num_points;
        this->results_file << this->query_file_data->num_dimensions;
        this->results_file << this->query_file_data->num_neighbors;
    }

    void generateResultsFile(std::vector<std::vector<KNNQueue>>& results) {
        this->writeFileHeader();

        for (auto map_it = results.begin(); map_it != results.end(); map_it++) {
            auto chunk = *map_it;

            for (KNNQueue& result : chunk) {
                this->generateResultsFile(result);
            }
        }

        this->results_file.close();
    }

    void generateResultsFile(KNNQueue& results) {
        this->writeQueryResults(results);

        this->results_file.close();
    }

    void writeQueryResults(KNNQueue& nearest_neighbors) {
//         printf("%s:%d Farthest point is %f\n", __FILE__, __LINE__, nearest_neighbors->top().point[0]);
        std::stack<Neighbor> neighbors;
//        std::stack<float*> neighbors;

        while (!nearest_neighbors.empty()) {
            neighbors.push(nearest_neighbors.top());
            nearest_neighbors.pop();
        }

        while (!neighbors.empty()) {
            // TODO fix this
//            float* point = neighbors.top().point;
//            printf("%f, ", neighbors.top().distance_from_queried_point);
//            for (uint64_t i = 0; i < this->query_file_data->num_dimensions; ++i) {
//                printf("%llu:%f ", i, point[i]);
//            }
//            printf("\n");
            this->results_file << nearest_neighbors.top().point;
//            this->results_file << nearest_neighbors.top();
            // nearest_neighbors->top().point->printCoordinates();
            neighbors.pop();
        }

//        printf("\n");

//        delete nearest_neighbors;
    }
};


#endif
