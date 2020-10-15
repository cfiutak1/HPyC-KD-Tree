#include <cstdint>

#pragma once


class FileData {
public:
    std::size_t file_id = 0;
    std::size_t num_points = 0;
    std::size_t num_dimensions = 0;

    FileData() = default;

    FileData(std::size_t file_id_in, std::size_t num_points_in, std::size_t num_dimensions_in):
        file_id(file_id_in),
        num_points(num_points_in),
        num_dimensions(num_dimensions_in)
    {}
};
