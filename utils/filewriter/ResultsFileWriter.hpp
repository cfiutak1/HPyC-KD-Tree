#include "../../src/kdtree/KDTree.hpp"
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

    template <typename ItemT>
    void writeQueryResults(hpyc::KDTree<ItemT>& tree, std::size_t* indices) {
        ItemT* point_to_write = new ItemT[this->query_file_data->num_dimensions];

        for (std::size_t i = 0; i < this->query_file_data->num_neighbors; i++) {
            tree.readPointAt(point_to_write, indices[i]);

            this->results_file.write(
                reinterpret_cast<char*>(&(point_to_write)),
                sizeof(ItemT) * this->query_file_data->num_dimensions
            );
        }

        delete[] point_to_write;
    }
};
