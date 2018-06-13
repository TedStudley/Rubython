#ifndef HEADER__RB_PY_LIST_H__INCLUDED
#define HEADER__RB_PY_LIST_H__INCLUDED

#include "ruby.h"
#include "Python.h"
#include "listobject.h"

typedef PyListObject rb_Rubython_PyList;

extern VALUE rb_cPyList;

static void rb_cRubython_PyList__mark(void *ptr);
static void rb_cRubython_PyList__free(void *ptr);
static size_t rb_cRubython_PyList__memsize(const void *ptr);

extern const rb_data_type_t rb_Rubython_PyList_type;

VALUE PyList_WRAP(PyObject *ptr);
static inline VALUE RbListWrap(PyObject *ptr) {
  DEBUG_MARKER;

  // TODO: This can blow up if ptr doesn't point to a Python object (Do we care?)
  if (!PyList_Check(ptr))
    rb_raise(rb_eTypeError, "Expected python list type, but got %s",
        Py_TYPE(ptr)->tp_name);

  return PyList_WRAP(ptr);
}

#define PYLIST_PTR(obj) \
  (PyListObject *)rb_check_typeddata((obj), &rb_Rubython_PyList_type)
#define PYLIST_OBJPTR(obj) \
  (PyObject *)rb_check_typeddata((obj), &rb_Rubython_PyList_type)

static VALUE rb_cRubython_PyList_to_ary(VALUE self);
static VALUE rb_cRubython_PyList_aref(VALUE self, VALUE index);
static VALUE rb_cRubython_PyList_aset(VALUE self, VALUE index, VALUE value);
static VALUE rb_cRubython_PyList_eq(VALUE self, VALUE other);

void Init_PyList(void);

#endif // HEADER__RB_PY_LIST_H__INCLUDED
