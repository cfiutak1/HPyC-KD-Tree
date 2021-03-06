#include "../filedata/FileData.hpp"

#include <string>
#include <fstream>

#pragma once


class FileProcessor {
protected:
    std::string file_name;
    std::ifstream input_stream;
    FileData* file_data;

public:
    FileProcessor(std::string file_name): input_stream(file_name, std::ios_base::in | std::ios_base::binary) {
        this->file_name = file_name;
    }

    void close() {
        if (this->input_stream) { this->input_stream.close(); }
    }

    float* readPoints1D() {
        std::size_t num_points = this->file_data->num_points;
        float* points = new float[num_points * this->file_data->num_dimensions];

        for (std::size_t i = 0; i < num_points; ++i) {
            for (std::size_t j = 0; j < this->file_data->num_dimensions; ++j) {
                float* f = points + ((num_points * j) + i);

                this->input_stream.read(
                    (char*) f,
                    sizeof(float)
                );
            }
        }

        return points;
    }

    float** readPointsRowCol() {
        std::size_t num_points = this->file_data->num_points;

        float** points = new float*[num_points];

        for (std::size_t i = 0; i < num_points; i++) {
            if (this->input_stream) {
                float* f = new float[this->file_data->num_dimensions];

                this->input_stream.read(
                    (char*) f,
                    this->file_data->num_dimensions * sizeof(float)
                );

                points[i] = (f);
            }
        }

        this->close();

        return points;
    }

    float** readPointsColRow() {
        float** points = new float*[this->file_data->num_dimensions];

        for (std::size_t i = 0; i < this->file_data->num_dimensions; ++i) {
            points[i] = new float[this->file_data->num_points];
        }


        float* read_buffer = new float[this->file_data->num_dimensions];

        for (std::size_t i = 0; i < this->file_data->num_points; ++i) {
            this->input_stream.read(
                (char*) read_buffer,
                this->file_data->num_dimensions * sizeof(float)
            );

            for (std::size_t j = 0; j < this->file_data->num_dimensions; ++j) {
                points[j][i] = read_buffer[j];
            }
        }

        delete[] read_buffer;

        return points;
    }
};
