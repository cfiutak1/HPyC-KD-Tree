#include <cstdint>

#pragma once


class FileData {
public:
    uint64_t file_id = 0;
    uint64_t num_points = 0;
    uint64_t num_dimensions = 0;

    FileData() = default;

    FileData(uint64_t file_id_in, uint64_t num_points_in, uint64_t num_dimensions_in):
        file_id(file_id_in),
        num_points(num_points_in),
        num_dimensions(num_dimensions_in)
    {}
};
