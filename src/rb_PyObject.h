#ifndef HEADER__RB_PY_OBJECT_H__INCLUDED
#define HEADER__RB_PY_OBJECT_H__INCLUDED

#include "ruby.h"
#include "Python.h"
#include "object.h"

extern VALUE rb_mPyTypes, rb_cPyObject;

typedef PyObject rb_Rubython_PyObject;

static void rb_cRubython_PyObject__mark(void *ptr);
static void rb_cRubython_PyObject__free(void *ptr);
static size_t rb_cRubython_PyObject__memsize(const void *ptr);

extern const rb_data_type_t rb_Rubython_PyObject_type;

VALUE PyObject_WRAP(PyObject *ptr);
static inline VALUE PyObject_Wrap(PyObject *ptr) {
  DEBUG_MARKER;
  return PyObject_WRAP(ptr);
}

#define PyObject_PTR_OF(rval,type)\
  (PyObject *)rb_check_typeddata((rval),(type))
#define PyObject_PTR(rval)\
  PyObject_PTR_OF((rval),&rb_Rubython_PyObject_type)

static VALUE rb_cRubython_PyObject_initialize(VALUE self);

static VALUE rb_cRubython_PyObject_to_s(VALUE self);
static VALUE rb_cRubython_PyObject_inspect(VALUE self);

static VALUE rb_cRubython_PyObject_methods(int argc, const VALUE *argv, VALUE self);
static VALUE rb_cRubython_PyObject_hasattr(VALUE self, VALUE key);
static VALUE rb_cRubython_PyObject_getattr(VALUE self, VALUE key);
static VALUE rb_cRubython_PyObject_call(VALUE self, VALUE args);

static VALUE rb_cRubython_String_to_py(VALUE self);
static VALUE rb_cRubython_Array_to_py(VALUE self);
static VALUE rb_cRubython_Dict_to_py(VALUE self);

void Init_PyObject(void);

#endif // HEADER__RB_PY_OBJECT_H__INCLUDED
