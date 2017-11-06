#ifndef HEADER__PY_RB_ARRAY_H__INCLUDED
#define HEADER__PY_RB_ARRAY_H__INCLUDED

#include "py_RbObject.h"

typedef union py_Rubython_RbArrayStruct {
  py_Rubython_RbObject object;
  struct {
    PyObject_HEAD
    union {
      VALUE value;
      struct RArray *rb_array;
    } as;
  } array;
} py_Rubython_RbArray;

extern PyObject *py_cRbArray;
extern PyTypeObject py_Rubython_RbArray_type;

PyObject *
py_cRubython_RbArray_s_wrap(PyTypeObject *type, VALUE obj);
PyObject *RbArrayWrap(VALUE obj);

void init_Rubython_RbArray(void);

#endif // HEADER__PY_RB_ARRAY_H__INCLUDED
