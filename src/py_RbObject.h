#ifndef HEADER__PY_RB_OBJECT_H__INCLUDED
#define HEADER__PY_RB_OBJECT_H__INCLUDED

#include "python/structmember.h"

typedef struct  py_Rubython_RbObjectStruct {
  PyObject_HEAD;
  union {
    VALUE value;
    struct RObject *rb_object;
  } as;
} py_Rubython_RbObject;

extern PyObject *py_cRbObject;
extern PyTypeObject py_Rubython_RbObject_type;

static void
py_cRubython_RbObject_s_dealloc(py_Rubython_RbObject *self);

static PyObject *
py_cRubython_RbObject_getattr(py_Rubython_RbObject *self, const char *attr_name);
static PyObject *
py_cRubython_RbObject_setattr(py_Rubython_RbObject *self, const char *attr_name, PyObject *value);

static PyObject *
py_cRubython_RbObject_repr(py_Rubython_RbObject *self);

static PyObject *
py_cRubython_RbObject_call(py_Rubython_RbObject *self, PyObject *args, PyObject *other);

static PyObject *
py_cRubython_RbObject_str(py_Rubython_RbObject *self);

static int
py_cRubython_RbObject_s_clear(py_Rubython_RbObject *self);

PyObject *
py_cRubython_RbObject_s_wrap(PyTypeObject *type, VALUE obj);
PyObject *RbObjectWrap(VALUE obj);

static PyObject *
py_cRubython_RbObject_s_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

static int
py_cRubython_RbObject_init(py_Rubython_RbObject *self, PyObject *args, PyObject *kwds);

void init_Rubython_RbObject(void);

#endif // HEADER__PY_RB_OBJECT_H__INCLUDED
