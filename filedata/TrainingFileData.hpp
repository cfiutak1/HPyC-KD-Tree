#ifndef TRAININGFILEDATA_HPP
#define TRAININGFILEDATA_HPP

#include "FileData.hpp"


class TrainingFileData : public FileData {
public:
    TrainingFileData(uint64_t file_id_in, uint64_t num_points_in, uint64_t num_dimensions_in):
        FileData(file_id_in, num_points_in, num_dimensions_in)
    {}
};

#endif
