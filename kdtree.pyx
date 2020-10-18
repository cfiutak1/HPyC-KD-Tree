# distutils: language = c++
# cimport numpy as np
import time
import numpy as np
from cython cimport view
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


cdef extern from "kdtree/KDTree.hpp" namespace "hpyc":
    cdef cppclass KDTree[ItemT]:
        size_t num_dimensions
        KDTree(ItemT* nodes_in, const size_t num_points_in, const size_t num_dimensions_in) except +
        void nearestNeighborsSearch(ItemT* query_point, const size_t num_neighbors, long* indices, double* distances)



cdef class PyLongKDTree:
    cdef KDTree[long]* thisptr

    def __cinit__(self, long[:, :] nodes_in):
        self.thisptr = new KDTree[long](&nodes_in[0][0], nodes_in.shape[0], nodes_in.shape[1])

    def __dealloc__(self):
        del self.thisptr

    def nearest_neighbors(self, long[::1] query_point, long num_neighbors):
        cdef long[::1] indices = np.empty(num_neighbors, dtype=np.int64)
        cdef double[::1] distances = np.empty(num_neighbors, dtype=np.float64)

        self.thisptr.nearestNeighborsSearch(&query_point[0], num_neighbors, &indices[0], &distances[0])

        return indices, distances



cdef class PyDoubleKDTree:
    cdef KDTree[double]* thisptr

    def __cinit__(self, double[:, :] nodes_in):
        self.thisptr = new KDTree[double](&nodes_in[0][0], nodes_in.shape[0], nodes_in.shape[1])

    def __dealloc__(self):
        del self.thisptr

    def nearest_neighbors(self, double[::1] query_point, long num_neighbors):
        cdef long[::1] indices = np.empty(num_neighbors, dtype=np.int64)
        cdef double[::1] distances = np.empty(num_neighbors, dtype=np.float64)

        self.thisptr.nearestNeighborsSearch(&query_point[0], num_neighbors, &indices[0], &distances[0])

        return indices, distances




class PyKDTree:
    def __init__(self, nodes_in):
        t = time.time()
        nodes_in = np.asfortranarray(nodes_in)
        print(f"Fortran transform time {time.time() - t}")

        if nodes_in.dtype == np.float64:
            self.kdtree = PyDoubleKDTree(nodes_in)

        elif nodes_in.dtype == np.int64:
            self.kdtree = PyLongKDTree(nodes_in)

        else:
            print(nodes_in.dtype)
            raise TypeError("...")

    def nearest_neighbors(self, query_point, num_neighbors):
        return self.kdtree.nearest_neighbors(query_point, num_neighbors)

