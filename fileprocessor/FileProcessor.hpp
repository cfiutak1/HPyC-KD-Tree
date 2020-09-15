#ifndef FILEPROCESSOR_HPP
#define FILEPROCESSOR_HPP

#include <string>
#include <fstream>
#include <vector>


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
        if (this->input_stream) this->input_stream.close();
    }

    float** readPoints() {
        uint64_t num_points = this->file_data->num_points;
        alignas(64) float** points = new float*[num_points];

        for (uint64_t i = 0; i < num_points; i++) {
            if (this->input_stream) {
                float* f = new float[this->file_data->num_dimensions];

//                for (unsigned int j = 0; j < this->file_data->num_dimensions; ++j) {
//                    printf("%lu ", &(f[j]));
//                }
//
//                printf("\n");

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

        for (auto i = 0; i < this->file_data->num_dimensions; ++i) {
            points[i] = new float[this->file_data->num_points];
        }


        float* read_buffer = new float[this->file_data->num_dimensions];

        for (auto i = 0; i < this->file_data->num_points; ++i) {
            this->input_stream.read(
                (char*) read_buffer,
                this->file_data->num_dimensions * sizeof(float)
            );

            for (auto j = 0; j < this->file_data->num_dimensions; ++j) {
                points[j][i] = read_buffer[j];
            }
        }

        delete[] read_buffer;

        return points;
    }
};

#endif
