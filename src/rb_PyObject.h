#ifndef HEADER__RB_PY_OBJECT_H__INCLUDED
#define HEADER__RB_PY_OBJECT_H__INCLUDED

#include "ruby.h"
#include "python/Python.h"
#include "python/object.h"

extern VALUE rb_mPyTypes, rb_cPyObject;

typedef PyObject rb_Rubython_PyObject;

static void rb_cRubython_PyObject__mark(void *ptr);
static void rb_cRubython_PyObject__free(void *ptr);
static size_t rb_cRubython_PyObject__memsize(const void *ptr);

extern const rb_data_type_t rb_Rubython_PyObject_type;

VALUE rb_cRubython_PyObject__wrap(void *ptr);

static VALUE rb_cRubython_PyObject_initialize(int argc, VALUE *argv, VALUE self);

VALUE rb_cRubython_PyObject_to_s(VALUE self);
VALUE rb_cRubython_PyObject_inspect(VALUE self);

VALUE rb_cRubython_PyObject_call(VALUE self, VALUE args);
VALUE rb_cRubython_PyObject_getattr(VALUE self, VALUE args);
VALUE rb_cRubython_PyObject_py_send(VALUE self, VALUE rb_key);

void Init_PyObject();

#endif // HEADER__RB_PY_OBJECT_H__INCLUDED
