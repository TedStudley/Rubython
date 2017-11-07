#ifndef HEADER__RB_PY_STRING_H__INCLUDED
#define HEADER__RB_PY_STRING_H__INCLUDED

#include "ruby.h"
#include "python/Python.h"
#include "python/stringobject.h"

extern VALUE rb_mPyTypes, rb_cPyString;

typedef PyStringObject rb_Rubython_PyString;

static void rb_cRubython_PyString__mark(void *ptr);
static void rb_cRubython_PyString__free(void *ptr);
static size_t rb_cRubython_PyString__memsize(const void *ptr);

extern const rb_data_type_t rb_Rubython_PyString_type;

VALUE rb_cRubython_PyString__wrap(void *obj);

VALUE rb_cRubython_PyString_to_s(VALUE self);
VALUE rb_cRubython_PyString_inspect(VALUE self);

void Init_py_string();

#endif // HEADER__RB_PY_STRING_H__INCLUDED
