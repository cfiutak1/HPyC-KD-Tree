#include "FileProcessor.hpp"

#include <string>

#pragma once


class QueryFileProcessor : public FileProcessor {
public:
    explicit QueryFileProcessor(std::string file_name): FileProcessor(file_name) {}

    QueryFileData* readQueryFileHeader() {
        char file_type[8];
        this->input_stream.read(reinterpret_cast<char *>(&file_type), 8);

        uint64_t file_id = 0;
        this->input_stream.read(reinterpret_cast<char *>(&file_id), 8);

        uint64_t num_points = 0;
        this->input_stream.read(reinterpret_cast<char *>(&num_points), 8);

        uint64_t num_dimensions = 0;
        this->input_stream.read(reinterpret_cast<char *>(&num_dimensions), 8);

        uint64_t num_neighbors = 0;
        this->input_stream.read(reinterpret_cast<char *>(&num_neighbors), 8);

        this->file_data = new QueryFileData(file_id, num_points, num_dimensions, num_neighbors);

        return static_cast<QueryFileData*>(this->file_data);
    }
};
