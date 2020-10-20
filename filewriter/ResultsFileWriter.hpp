//#include "../kdtree/Neighbor.hpp"
#include "../kdtree/KNNQueue.hpp"
#include "../filedata/FileData.hpp"
#include "../filedata/TrainingFileData.hpp"
#include "../filedata/QueryFileData.hpp"

#include <fstream>
#include <string>
#include <stack>
#include <algorithm>


class ResultsFileWriter {
private:
    std::string file_name;
    TrainingFileData* training_file_data;
    QueryFileData* query_file_data;

    std::ofstream results_file;

    std::size_t generateFileID() {
        std::ifstream urandom("/dev/urandom", std::ios::in|std::ios::binary);
        std::size_t rand = 0;

        if (urandom) {
            urandom.read(reinterpret_cast<char*>(&rand), 8);
            urandom.close();
        }

        return 69;
//        return rand;
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
        char file_type[] = "RESULT\0\0";
        auto file_id = this->generateFileID();
        this->results_file.write(reinterpret_cast<const char*>(&file_type), 8);
        this->results_file.write(reinterpret_cast<const char*>(&this->training_file_data->file_id), 8);
        this->results_file.write(reinterpret_cast<const char*>(&this->query_file_data->file_id), 8);
        this->results_file.write(reinterpret_cast<const char*>(&file_id), 8);
        this->results_file.write(reinterpret_cast<const char*>(&(this->query_file_data->num_points)), 8);
        this->results_file.write(reinterpret_cast<const char*>(&(this->query_file_data->num_dimensions)), 8);
        this->results_file.write(reinterpret_cast<const char*>(&(this->query_file_data->num_neighbors)), 8);
    }


//    void writeQueryResults(KNNQueue& nearest_neighbors) {
//        std::sort(nearest_neighbors.array, nearest_neighbors.array + this->query_file_data->num_neighbors, std::greater<>());
//
//        for (std::size_t i = this->query_file_data->num_neighbors; i > 0; --i) {
//            for (std::size_t j = 0; j < this->query_file_data->num_dimensions; ++j) {
//                this->results_file.write(reinterpret_cast<const char*>(&(nearest_neighbors.array[i - 1].point[j])), 4);
//            }
//
//        }
//    }
};
