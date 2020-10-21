#include "FileData.hpp"

#pragma once


class TrainingFileData : public FileData {
public:
    TrainingFileData(std::size_t file_id_in, std::size_t num_points_in, std::size_t num_dimensions_in):
        FileData(file_id_in, num_points_in, num_dimensions_in)
    {}
};
