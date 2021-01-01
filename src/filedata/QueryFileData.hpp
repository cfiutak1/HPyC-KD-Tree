#include "FileData.hpp"

#pragma once


class QueryFileData : public FileData {
public:
    std::size_t num_neighbors = 0;

    QueryFileData() = default;

    QueryFileData(std::size_t file_id, std::size_t num_queries, std::size_t num_dimensions, std::size_t num_neighbors_in):
        FileData(file_id, num_queries, num_dimensions),
        num_neighbors(num_neighbors_in)
    {}
};
