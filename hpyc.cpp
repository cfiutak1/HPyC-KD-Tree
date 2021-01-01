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
    hpyc::KDTree<float> tree;
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

    return 0;
}


static void KDTree_dealloc(KDTree* self) {
    Py_XDECREF(self->np_array);
    Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
}