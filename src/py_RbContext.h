#ifndef HEADER__PY_RB_CONTEXT_H__INCLUDED
#define HEADER__PY_RB_CONTEXT_H__INCLUDED

#include "Python.h"
#include "ruby.h"

typedef struct py_Rubython_RbContextStruct {
  PyObject_HEAD;
  PyObject *filename;
  VALUE rb_binding;

  struct py_Rubython_RbContext_Objects {
    PyObject **objects;
    unsigned packed;
    size_t size;
    size_t maxsize;
  } rb_objects;
} py_Rubython_RbContext;
#define RBCONTEXT_FILENAME(ptr) (((py_Rubython_RbContext *)(ptr))->filename)
#define RBCONTEXT_BINDING(ptr) (((py_Rubython_RbContext *)(ptr))->rb_binding)
#define RBCONTEXT_OBJECTS(ptr) (((py_Rubython_RbContext *)(ptr))->rb_objects)

extern PyObject *py_cRbContext;
extern PyTypeObject py_rubython_RbContext_type;

extern PyObject **rbcontext_instance_p;
static int py_cRubython_RbContext_tp_traverse(PyObject *self, visitproc visit, void *arg);
static void py_cRubython_RbContext_tp_dealloc(PyObject *self);
static int py_cRubython_RbContext_tp_clear(PyObject *self);
static void py_cRubython_RbContext_tp_free(void *self);
static PyObject *py_cRubython_RbContext_tp_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
static int py_cRubython_RbContext_tp_init(PyObject *self, PyObject *args, PyObject *kwds);
static PyObject *py_cRubython_RbContext_tp_getattr(PyObject *self, const char *attr_name);
static PyObject *py_cRubython_RbContext_tp_setattr(PyObject *self, const char *attr_name, PyObject *value);

int py_cRubython_RbContext_addObject(PyObject *self, PyObject *obj);
int py_cRubython_RbContext_removeObject(PyObject *self, PyObject *obj);

static PyObject *py_cRubython_RbContext_s_get_instance(PyObject *cls, PyObject *dummy);
static PyObject *py_cRubython_RbContext_s_finalize(PyObject *cls, PyObject *dummy);

PyObject *py_cRubython_RbContext_is_valid(PyObject *self);

void init_Rubython_RbContext(void);

#endif // HEADER__PY_RB_CONTEXT_H__INCLUDED
