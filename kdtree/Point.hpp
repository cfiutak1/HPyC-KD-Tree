#ifndef POINT_HPP
#define POINT_HPP

#include <cstdint>
#include <cstdio>
#include <cmath>
#include <cassert>


class Point {
public:
    float* coordinates;
    uint64_t num_dimensions;

    Point() {}

    Point(uint64_t num_dimensions) {
        this->coordinates = new float[num_dimensions];
        this->num_dimensions = num_dimensions;
    }

    ~Point() {
        // printf("Called point destructor\n");
        delete[] this->coordinates;
    }

    inline float getCoordinate(uint64_t dimension) const { return this->coordinates[dimension % this->num_dimensions]; }

    void printCoordinates() const {
        for (uint64_t i = 0; i < this->num_dimensions; i++) printf("%lu:%f ", i, *(this->coordinates + i));
        printf("\n");
    }

    float distanceBetween(Point* p) const {
        float distance = 0.0;

        for (uint64_t i = 0; i < this->num_dimensions; i++) {
            float diff_between_dimensions = this->getCoordinate(i) - p->getCoordinate(i);
            distance += pow(diff_between_dimensions, 2);
        }

        return distance;
    }
};


class PointDimensionDistanceComparator {
public:
    uint64_t dimension;

    PointDimensionDistanceComparator() {}
    PointDimensionDistanceComparator(uint64_t d) : dimension(d) {}

    bool operator()(Point* p1, Point* p2) {
        return p1->getCoordinate(dimension) < p2->getCoordinate(dimension);
    }
};


class PointTotalDistanceComparator {
private:
    Point* reference_point;

public:
    PointTotalDistanceComparator() {}
    PointTotalDistanceComparator(Point* p) : reference_point(p) {}

    bool operator()(Point* p1, Point* p2) {
        return this->reference_point->distanceBetween(p1) < this->reference_point->distanceBetween(p2);
    }
};


inline bool comp_lt(const Point* p1, const Point* p2, const unsigned long& dimension) {
    return p1->getCoordinate(dimension) < p2->getCoordinate(dimension);
}

inline bool comp_gt(const Point* p1, const Point* p2, const unsigned long& dimension) {
    return p1->getCoordinate(dimension) > p2->getCoordinate(dimension);
}

inline bool comp_lte(const Point* p1, const Point* p2, const unsigned long& dimension) {
    return p1->getCoordinate(dimension) <= p2->getCoordinate(dimension);
}

inline bool comp_gte(const Point* p1, const Point* p2, const unsigned long& dimension) {
    return p1->getCoordinate(dimension) >= p2->getCoordinate(dimension);
}

#endif
