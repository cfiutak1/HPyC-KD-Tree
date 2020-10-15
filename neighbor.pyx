# distutils: language = c++
from libcpp cimport bool


cdef extern from "kdtree/Neighbor.hpp" namespace "hpyc":
    cdef cppclass Neighbor:
        Neighbor() except +
        Neighbor(size_t, double) except +

        size_t index
        double distance_from_queried_point

        bool operator<(const Neighbor&)


cdef class PyNeighbor:
    cdef Neighbor* thisptr

    def __cinit__(self, size_t index_in, double distance_from_queried_point_in):
        self.thisptr = new Neighbor(index_in, distance_from_queried_point_in)

    def __dealloc__(self):
        del self.thisptr

