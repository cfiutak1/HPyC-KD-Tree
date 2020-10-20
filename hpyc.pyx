# distutils: language = c++

import time
import numpy as np
from cython cimport view
from libcpp cimport bool


cdef extern from "kdtree/KDTree.hpp" namespace "hpyc":
    cdef cppclass KDTree[ItemT]:
        size_t num_dimensions
        KDTree(ItemT* nodes_in, const size_t num_points_in, const size_t num_dimensions_in) except +
        void nearestNeighborsSearch(ItemT* query_point, const size_t num_neighbors, long* indices, double* distances)
        void readPointAt(ItemT* point, const size_t index)


cdef class NumpyFloatKDTree:
    cdef KDTree[float]* thisptr

    def __cinit__(self, float[:, :] nodes_in):
        self.thisptr = new KDTree[float](&nodes_in[0][0], nodes_in.shape[0], nodes_in.shape[1])

    def __dealloc__(self):
        del self.thisptr

    def nearest_neighbors(self, float[::1] query_point, long num_neighbors):
        cdef long[::1] indices = np.empty(num_neighbors, dtype=np.int64)
        cdef double[::1] distances = np.empty(num_neighbors, dtype=np.float64)

        self.thisptr.nearestNeighborsSearch(&query_point[0], num_neighbors, &indices[0], &distances[0])

        return np.asarray(indices), np.asarray(distances)

    def get_point(self, const size_t index):
        cdef float[::1] point = np.empty(self.thisptr.num_dimensions, dtype=np.float32)
        self.thisptr.readPointAt(&point[0], index)

        return np.asarray(point)


cdef class NumpyDoubleKDTree:
    cdef KDTree[double]* thisptr

    def __cinit__(self, double[:, :] nodes_in):
        self.thisptr = new KDTree[double](&nodes_in[0][0], nodes_in.shape[0], nodes_in.shape[1])

    def __dealloc__(self):
        del self.thisptr

    def nearest_neighbors(self, double[::1] query_point, long num_neighbors):
        cdef long[::1] indices = np.empty(num_neighbors, dtype=np.int64)
        cdef double[::1] distances = np.empty(num_neighbors, dtype=np.float64)

        self.thisptr.nearestNeighborsSearch(&query_point[0], num_neighbors, &indices[0], &distances[0])

        return np.asarray(indices), np.asarray(distances)


class PyKDTree:
    def __init__(self, nodes_in):
        nodes_in = np.asfortranarray(nodes_in)
        self.nodes_in = nodes_in

        if nodes_in.dtype == np.float32:
            self.kdtree = NumpyFloatKDTree(self.nodes_in)

        elif nodes_in.dtype == np.float64:
            self.kdtree = NumpyDoubleKDTree(self.nodes_in)

        else:
            raise TypeError(f"Unsupported type {type(nodes_in)}")

    def nearest_neighbors(self, query_point, num_neighbors):
        return self.kdtree.nearest_neighbors(query_point, num_neighbors)
