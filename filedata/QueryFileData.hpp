#include "FileData.hpp"

#pragma once


class QueryFileData : public FileData {
public:
    uint64_t num_neighbors = 0;

    QueryFileData() = default;

    QueryFileData(uint64_t file_id, uint64_t num_queries, uint64_t num_dimensions, uint64_t num_neighbors_in):
        FileData(file_id, num_queries, num_dimensions),
        num_neighbors(num_neighbors_in)
    {}
};
