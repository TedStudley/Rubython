#ifndef HEADER__PY_RB_CONTEXT_H__INCLUDED
#define HEADER__PY_RB_CONTEXT_H__INCLUDED

#include "Python.h"
#include "ruby.h"

typedef struct py_Rubython_RbContextStruct {
  PyObject_HEAD;
  PyObject *filename;
  VALUE rb_binding;
} py_Rubython_RbContext;

extern PyObject *py_cRbContext;
extern PyTypeObject py_rubython_RbContext_type;

static void
py_cRubython_RbContext_s_dealloc(py_Rubython_RbContext *self);

static int
py_cRubython_RbContext_s_clear(py_Rubython_RbContext *self);

static PyObject *
py_cRubython_RbContext_s_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

static PyObject *
py_cRubython_RbContext_s_finalize(py_Rubython_RbContext *cls, PyObject *dummy);

static int
py_cRubython_RbContext_init(py_Rubython_RbContext *self, PyObject *args, PyObject *kwds);

void
init_Rubython_RbContext(void);

#endif // HEADER__PY_RB_CONTEXT_H__INCLUDED
