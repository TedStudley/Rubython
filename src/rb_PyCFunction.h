#ifndef HEADER__RB_PY_C_FUNCTION_H__INCLUDED
#define HEADER__RB_PY_C_FUNCTION_H__INCLUDED

#include "ruby.h"
#include "python/Python.h"
#include "python/methodobject.h"

extern VALUE rb_mPyTypes, rb_cPyCFunction;

typedef PyCFunctionObject rb_Rubython_PyCFunction;

static void rb_cRubython_PyCFunction__mark(void *ptr);
static void rb_cRubython_PyCFunction__free(void *ptr);
static size_t rb_cRubython_PyCFunction__memsize(const void *ptr);

extern const rb_data_type_t rb_Rubython_PyCFunction_type;

VALUE rb_cRubython_PyCFunction__wrap(void *obj);

void Init_PyCFunction();

#endif // HEADER__RB_PY_C_FUNCTION_H__INCLUDED
