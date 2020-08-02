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
};

#endif
