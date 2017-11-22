#ifndef HEADER__PY_RB_STRING_H__INCLUDED
#define HEADER__PY_RB_STRING_H__INCLUDED

#include "Python.h"
#include "structmember.h"
#include "ruby.h"

typedef struct py_Rubython_RbStringStruct {
  PyRbObject_HEAD;
  union {
    VALUE value;
    struct RString *ptr;
  } as;
} py_Rubython_RbString;
#define RBSTRING_PTR(ptr) (((py_Rubython_RbString *)(ptr))->as.ptr)
#define RBSTRING_VALUE(ptr) (((py_Rubython_RbString *)(ptr))->as.value)

extern PyObject *py_cRbString;
extern PyTypeObject py_Rubython_RbString_type;

Py_ssize_t py_cRubython_RbString_bf_getreadbuffer(py_Rubython_RbString *self, Py_ssize_t segment, void **ptrptr);
Py_ssize_t py_cRubython_RbString_bf_getsegcount(py_Rubython_RbString *self, Py_ssize_t *lenp);
Py_ssize_t py_cRubython_RbString_bf_getcharbuffer(py_Rubython_RbString *self, Py_ssize_t segment, char **ptrptr);

PyObject *RbString_WRAP(VALUE ptr);
PyObject *RbString_Wrap(VALUE ptr);

static PyObject *
py_cRubython_RbString_tp_richcompare(PyObject *self, PyObject *other, int opid);
static long
py_cRubython_RbString_tp_hash(py_Rubython_RbString *self);
static PyObject *
py_cRubython_RbString_tp_str(py_Rubython_RbString *self);

void init_Rubython_RbString(void);

#endif // HEADER__PY_RB_STRING_H__INCLUDED
