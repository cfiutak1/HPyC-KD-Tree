# distutils: language = c++
#cython: boundscheck=False


import numpy as np
from cython cimport view
from libcpp cimport bool


cdef extern from "src/kdtree/KDTree.hpp" namespace "hpyc":
    cdef cppclass KDTree[ItemT]:
        size_t num_dimensions
        KDTree(ItemT* nodes_in, const size_t num_points_in, const size_t num_dimensions_in) except +
        void nearestNeighborsSearch(ItemT* query_point, const size_t num_neighbors, size_t* indices, double* distances)
        void readPointAt(ItemT* point, const size_t index)


# cdef extern from "src/parallel_kdtree/ParallelKDTree.hpp" namespace "hpyc":
#     cdef cppclass ParallelKDTree[ItemT]:
#         KDTree[ItemT]* tree
#         size_t num_dimensions
#         ParallelKDTree(ItemT* nodes_in, const size_t num_points_in, const size_t num_dimensions_in, const unsigned int num_threads) except +
#         void nearestNeighborsSearches(const ItemT* query_points, const size_t num_queries, const size_t num_neighbors, size_t* indices, double* distances)
#         void readPointAt(ItemT* point, const size_t index)


cdef class NumpyFloatKDTree:
    cdef KDTree[float]* thisptr

    def __cinit__(self, float[:, :] nodes_in):
        self.thisptr = new KDTree[float](&nodes_in[0][0], nodes_in.shape[0], nodes_in.shape[1])

    def __dealloc__(self):
        del self.thisptr

    def nearest_neighbors(self, float[::1] query_point, size_t num_neighbors):
        cdef size_t[::1] indices = np.empty(num_neighbors, dtype=np.uint64)
        cdef double[::1] distances = np.empty(num_neighbors, dtype=np.float64)

        self.thisptr.nearestNeighborsSearch(&query_point[0], num_neighbors, &indices[0], &distances[0])

        return np.asarray(indices), np.asarray(distances)

    def get_point(self, const size_t index):
        cdef float[::1] point = np.empty(self.thisptr.num_dimensions, dtype=np.float32)
        self.thisptr.readPointAt(&point[0], index)

        return np.asarray(point)


# cdef class NumpyFloatParallelKDTree:
#     cdef ParallelKDTree[float]* thisptr
#
#     def __cinit__(self, float[:, :] nodes_in, unsigned int num_threads):
#         self.thisptr = new ParallelKDTree[float](&nodes_in[0][0], nodes_in.shape[0], nodes_in.shape[1], num_threads)
#
#     def __dealloc__(self):
#         del self.thisptr
#
#     def nearest_neighbors(self, float[:, ::1] query_points, size_t num_queries, size_t num_neighbors):
#         cdef size_t[:, ::1] indices = np.empty(num_neighbors * num_queries, dtype=np.uint64)
#         cdef double[:, ::1] distances = np.empty([num_queries, num_neighbors], dtype=np.float64)
#
#         self.thisptr.nearestNeighborsSearches(&query_points[0][0], num_queries, num_neighbors, &indices[0][0], &distances[0][0])
#
#         return np.asarray(indices), np.asarray(distances)
#
#     def get_point(self, const size_t index):
#         cdef float[::1] point = np.empty(self.thisptr.tree.num_dimensions, dtype=np.float32)
#         self.thisptr.tree.readPointAt(&point[0], index)
#
#         return np.asarray(point)


# cdef class NumpyDoubleKDTree:
#     cdef KDTree[double]* thisptr
#
#     def __cinit__(self, double[:, :] nodes_in):
#         self.thisptr = new KDTree[double](&nodes_in[0][0], nodes_in.shape[0], nodes_in.shape[1])
#
#     def __dealloc__(self):
#         del self.thisptr
#
#     def nearest_neighbors(self, double[::1] query_point, size_t num_neighbors):
#         cdef size_t[::1] indices = np.empty(num_neighbors, dtype=np.uint64)
#         cdef double[::1] distances = np.empty(num_neighbors, dtype=np.float64)
#
#         self.thisptr.nearestNeighborsSearch(&query_point[0], num_neighbors, &indices[0], &distances[0])
#
#         return np.asarray(indices), np.asarray(distances)

class PyKDTree:
    def __init__(self, nodes_in):
        nodes_in = np.asfortranarray(nodes_in)
        self.nodes_in = nodes_in

        if nodes_in.dtype == np.float32:
            self.kdtree = NumpyFloatKDTree(self.nodes_in)

        else:
            raise TypeError(f"Unsupported type {type(nodes_in)}")

    def nearest_neighbors(self, query_point, num_neighbors):
        return self.kdtree.nearest_neighbors(query_point, num_neighbors)
