#include <cstdint>

#pragma once

class NeighborPointRecycler {
private:
    float* allocated_points;

    std::size_t num_dimensions;
    std::size_t num_neighbors;
    std::size_t num_allocated = 0;

public:
    float* potential_neighbor;

    NeighborPointRecycler() = delete;

    NeighborPointRecycler(std::size_t num_dimensions_in, std::size_t num_neighbors_in):
        num_dimensions(num_dimensions_in),
        num_neighbors(num_neighbors_in)
    {
        this->allocated_points = new float[num_dimensions_in * (num_neighbors_in + 1)];
        this->allocated_points = new (std::align_val_t(32)) float[num_dimensions_in * (num_neighbors_in + 1)];
        this->potential_neighbor = this->allocated_points + (num_dimensions_in * num_neighbors_in);
    }

    float* getPoint() {
        float* point = this->allocated_points + (num_allocated * num_dimensions);
        ++this->num_allocated;

        return point;
    }

    void resetCount() {
        this->num_allocated = 0;
    }

    ~NeighborPointRecycler() {
        delete[] this->allocated_points;
    }
};