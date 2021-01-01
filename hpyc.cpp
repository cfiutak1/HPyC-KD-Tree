#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "numpy/ndarraytypes.h"
#include "numpy/ufuncobject.h"
#include "numpy/npy_3kcompat.h"
#include "structmember.h"
#include <cstddef>

#include "src/kdtree/KDTree.hpp"


static PyObject* HPyCError;


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
    // TODO make this support various data types
    PyObject* array_obj;

#if NPY_API_VERSION >= 13
    array_obj = PyArray_FROM_OTF(np_array_in, NPY_FLOAT32, NPY_ARRAY_INOUT_ARRAY2);
#else
    array_obj = PyArray_FROM_OTF(np_array_in, NPY_FLOAT32, NPY_ARRAY_INOUT_ARRAY);
#endif

    PyArrayObject* np_array_obj = reinterpret_cast<PyArrayObject*>(array_obj);
    npy_float32* data = reinterpret_cast<npy_float32*>(np_array_obj->data);

    // TODO Dimensionality check
    npy_intp* shape = PyArray_SHAPE(np_array_obj);
    self->num_dimensions = shape[0];
    self->num_points = shape[1];

    self->tree = new KDTree<float>(data, self->num_points, self->num_dimensions);

    return 0;
}


static void KDTree_dealloc(KDTree* self) {
    Py_XDECREF(self->np_array);
    Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));


    delete self->tree;
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
    },
    {nullptr} // Sentinel
};


static inline PyTypeObject KDTree_type_definition() {
    static PyTypeObject type_definition = {
        PyVarObject_HEAD_INIT(nullptr, 0)
        "hpyc.KDTree",
        sizeof(KDTree)
    };

    type_definition.tp_doc = "Numpy array summer";
    type_definition.tp_basicsize = sizeof(KDTree);
    type_definition.tp_itemsize = 0;
    type_definition.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    type_definition.tp_new = PyType_GenericNew;
    type_definition.tp_init = reinterpret_cast<initproc>(KDTree_init);
    type_definition.tp_members = KDTree_members;
    type_definition.tp_methods = KDTree_methods;

    return type_definition;
}


static inline PyModuleDef hpyc_module_definition() {
    static PyModuleDef module = {PyModuleDef_HEAD_INIT};

    module.m_name = "hpyc";
    module.m_doc = "Just the KD tree, for now.";
    module.m_size = -1;

    return module;
}


static const PyModuleDef module_def = hpyc_module_definition();
static const PyTypeObject kdtree_type_definition = KDTree_type_definition();


PyMODINIT_FUNC PyInit_hpyc() {
    Py_Initialize();
    import_array();

    if (PyType_Ready(reinterpret_cast<_typeobject*>(&NumpyArraySummerType)) < 0) {
        return nullptr;
    }

    PyObject* module = PyModule_Create(&module_def);

    if (module == nullptr) {
        return nullptr;
    }

    HPyCError = PyErr_NewException("hpyc.HPyCError", nullptr, nullptr);
    Py_XINCREF(HPyCError);

    if (PyMOdule_AddObject(module, "Error creating module", HPyCError) < 0) {
        Py_XDECREF(HPyCError);
        Py_CLEAR(HPyCError);
        Py_DECREF(module);

        return nullptr;
    }

    Py_INCREF(&kdtree_type_definition);

    if (PyModule_AddObject(module, "KDTree", reinterpret_cast<PyObject*>(&kdtree_type_definition)) < 0) {
        Py_DECREF(&kdtree_type_definition);
        Py_DECREF(module);

        return nullptr;
    }

    return module;
}