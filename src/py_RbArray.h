#ifndef HEADER__PY_RB_ARRAY_H__INCLUDED
#define HEADER__PY_RB_ARRAY_H__INCLUDED

#include "Python.h"
#include "ruby.h"

#include "py_RbObject.h"

typedef struct py_Rubython_RbArrayStruct {
  PyRbObject_HEAD;
  union {
    VALUE value;
    struct RArray *ptr;
  } as;
} py_Rubython_RbArray;
#define RBARRAY_PTR(ptr) (((py_Rubython_RbArray *)(ptr))->as.ptr)
#define RBARRAY_VALUE(ptr) (((py_Rubython_RbArray *)(ptr))->as.value)

extern PyObject *py_cRbArray;
extern PyTypeObject py_Rubython_RbArray_type;

Py_ssize_t py_cRubython_RbArray_sq_length(PyObject *self);
PyObject *py_cRubython_RbArray_sq_item(PyObject *self, Py_ssize_t idx);
int py_cRubython_RbArray_sq_ass_item(PyObject *self, Py_ssize_t idx, PyObject *item);

PyObject *RbArray_WRAP(VALUE ptr);
PyObject *RbArray_Wrap(VALUE ptr);

PyObject *py_cRubython_RbArray_tp_richcompare(PyObject *self, PyObject *other, int op);

void init_Rubython_RbArray(void);

#endif // HEADER__PY_RB_ARRAY_H__INCLUDED
