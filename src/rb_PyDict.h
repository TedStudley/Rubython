#ifndef HEADER__RB_PY_DICT_H__INCLUDED
#define HEADER__RB_PY_DICT_H__INCLUDED

#include "ruby.h"
#include "Python.h"
#include "object.h"

typedef PyDictObject rb_Rubython_PyDict;

extern VALUE rb_cPyDict;

static void rb_cRubython_PyDict__mark(void *ptr);
static void rb_cRubython_PyDict__free(void *ptr);
static size_t rb_cRubython_PyDict__memsize(const void *ptr);

extern const rb_data_type_t rb_Rubython_PyDict_type;

VALUE PyDict_WRAP(PyObject *ptr);
static inline VALUE PyDict_Wrap(PyObject *ptr) {
  DEBUG_MARKER;

  if (!PyDict_Check(ptr))
    rb_raise(rb_eTypeError, "Expected python list type, but got %s",
        Py_TYPE(ptr)->tp_name);

  return PyDict_WRAP(ptr);
}

#define PYDICT_PTR(obj) \
  (PyDictObject *)rb_check_typeddata((obj), &rb_Rubython_PyDict_type)
#define PYDICT_OBJPTR(obj) \
  (PyObject *)rb_check_typeddata((obj), &rb_Rubython_PyDict_type)

static VALUE rb_cRubython_PyDict_to_hash(VALUE self);
static VALUE rb_cRubython_PyDict_aref(VALUE self, VALUE key);
static VALUE rb_cRubython_PyDict_aset(VALUE self, VALUE key, VALUE value);

void Init_PyDict();

#endif // HEADER__RB_PY_DICT_H__INCLUDED
