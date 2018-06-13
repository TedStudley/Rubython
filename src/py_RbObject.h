#ifndef HEADER__PY_RB_OBJECT_H__INCLUDED
#define HEADER__PY_RB_OBJECT_H__INCLUDED

#include "Python.h"
#include "structmember.h"
#include "ruby.h"

#define PyRbObject_HEAD \
  PyObject_HEAD;\
  PyObject *context

typedef struct py_Rubython_RbObjectStruct {
  PyRbObject_HEAD;
  union {
    VALUE value;
    struct RObject *ptr;
  } as;
} py_Rubython_RbObject;
#define RBOBJECT_CONTEXT(ptr) (((py_Rubython_RbObject *)(ptr))->context)
#define RBOBJECT_PTR(ptr) (((py_Rubython_RbObject *)(ptr))->as.ptr)
#define RBOBJECT_VALUE(ptr) (((py_Rubython_RbObject *)(ptr))->as.value)

extern PyObject *py_cRbObject;
extern PyTypeObject py_Rubython_RbObject_type;

int py_cRubython_RbObject_release(PyObject *self);
static int py_cRubython_RbObject_tp_traverse(PyObject *self, visitproc visit, void *arg);
static void py_cRubython_RbObject_tp_dealloc(PyObject *self);
static int py_cRubython_RbObject_tp_clear(PyObject *self);
static PyObject * py_cRubython_RbObject_tp_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
static int py_cRubython_RbObject_tp_init(PyObject *self, PyObject *args, PyObject *kwds);

static PyObject *py_cRubython_RbObject_tp_getattr(PyObject *self, const char *attr_name);

static PyObject *py_cRubython_RbObject_tp_call(PyObject *self, PyObject *args, PyObject *other);

static PyObject *py_cRubython_RbObject_tp_str(PyObject *self);
static PyObject *py_cRubython_RbObject_tp_repr(PyObject *self);

PyObject *RbObject_WRAP_BASE(PyTypeObject *type, VALUE ptr);
PyObject *RbObject_WRAP(VALUE ptr);

static PyObject *py_cRubython_RbObject_context(PyObject *self);

void init_Rubython_RbObject(void);

#endif // HEADER__PY_RB_OBJECT_H__INCLUDED
