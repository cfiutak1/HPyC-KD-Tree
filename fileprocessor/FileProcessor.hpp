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

    std::vector<float*> readPoints() {
        uint64_t num_points = this->file_data->num_points;
        std::vector<float*> points;
        points.reserve(num_points);

        for (uint64_t i = 0; i < num_points; i++) {
            if (this->input_stream) {
//                KDNode* n = new KDNode(this->file_data->num_dimensions);

                float* f = new float[this->file_data->num_dimensions];

                this->input_stream.read(
                    (char*) f,
                    this->file_data->num_dimensions * sizeof(float)
                );
//                printf("%s:%d ", __FILE__, __LINE__);
//
//                for (int i = 0; i < this->file_data->num_dimensions; ++i) {
//                    printf("%f ", f[i]);
//                }
//                printf("\n");
                points.push_back(f);
            }
        }

        return points;
    }
};

#endif
