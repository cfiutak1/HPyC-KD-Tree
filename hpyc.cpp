#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "numpy/ndarraytypes.h"
#include "numpy/ufuncobject.h"
#include "numpy/npy_3kcompat.h"
#include "structmember.h"
#include <cstddef>

#include "src/kdtree/KDTree.hpp"


typedef struct {
    PyObject_HEAD
    PyObject* np_array;
    hpyc::KDTree<float>* tree;
    std::size_t num_dimensions;
    std::size_t num_points;
} KDTree;


static int KDTree_init(KDTree* self, PyObject* args, PyObject* kwds) {
    PyObject* np_array_in;

    bool args_successfully_parsed = PyArg_ParseTuple(args, "O", &np_array_in);

    if (!args_successfully_parsed) {
        return -1;
    }

    if (np_array_in == nullptr) {
        PyErr_SetString(NumpyArraySummerError, "Invalid number of arguments passed to constructor");

        return -1;
    }

    PyObject* temp = self->np_array;
    Py_INCREF(np_array_in);
    self->np_array = np_array_in;
    Py_XDECREF(tmp);

    // TODO create tree
    // TODO initialize num_dimensions and num_points

    return 0;
}


static void KDTree_dealloc(KDTree* self) {
    Py_XDECREF(self->np_array);
    Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
    // TODO delete tree
}


static PyObject* KDTree_nearest_neighbors(KDTree* self, PyObject* num_neighbors) {
    // TODO
}


static PyMemberDef NumpyArraySummer_members[] = {
    {
        "np_array",
        T_OBJECT_EX,
        offsetof(KDTree, np_array),
        0,
        "The 2D numpy array containing the data points in column-major order."
    },
    {nullptr} // Sentinel
};


static PyMethodDef KDTree_methods[] = {
    {
        "nearest_neighbors",
        reinterpret_cast<PyCFunction>(KDTree_nearest_neighbors),
        METH_VARARGS,
        "Gets the nearest neighbors to a given point"
    }
};