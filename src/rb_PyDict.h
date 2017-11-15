#ifndef HEADER__RB_PY_DICT_H__INCLUDED
#define HEADER__RB_PY_DICT_H__INCLUDED

#include "ruby.h"
#include "Python.h"
#include "object.h"

extern VALUE rb_mPyTypes, rb_cPyDict;

typedef PyDictObject rb_Rubython_PyDict;

static void rb_cRubython_PyDict__mark(void *ptr);
static void rb_cRubython_PyDict__free(void *ptr);
static size_t rb_cRubython_PyDict__memsize(const void *ptr);

extern const rb_data_type_t rb_Rubython_PyDict_type;

VALUE rb_cRubython_PyDict__wrap(void *ptr);

static VALUE rb_cRubython_PyDict_initialize(int argc, VALUE *argv, VALUE self);

void Init_PyDict();

#endif // HEADER__RB_PY_DICT_H__INCLUDED
