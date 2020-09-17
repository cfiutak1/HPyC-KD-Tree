#include "FileProcessor.hpp"

#pragma once

class TrainingFileProcessor : public FileProcessor {
public:
    TrainingFileProcessor(std::string file_name): FileProcessor(file_name) {}

    TrainingFileData* readTrainingFileHeader() {
        char file_type[8];
        this->input_stream.read(reinterpret_cast<char *>(&file_type), 8);

        uint64_t file_id = 0;
        this->input_stream.read(reinterpret_cast<char *>(&file_id), 8);

        uint64_t num_points = 0;
        this->input_stream.read(reinterpret_cast<char *>(&num_points), 8);

        uint64_t num_dimensions = 0;
        this->input_stream.read(reinterpret_cast<char *>(&num_dimensions), 8);

        this->file_data = new TrainingFileData(file_id, num_points, num_dimensions);


        return static_cast<TrainingFileData*>(this->file_data);
    }
};
