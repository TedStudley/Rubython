#ifndef HEADER__RB_PY_FUNCTION_H__INCLUDED
#define HEADER__RB_PY_FUNCTION_H__INCLUDED

#include "ruby.h"
#include "Python.h"
#include "funcobject.h"

extern VALUE rb_mPyTypes, rb_cPyFunction;

typedef PyFunctionObject rb_Rubython_PyFunction;

static void rb_cRubython_PyFunction__mark(void *ptr);
static void rb_cRubython_PyFunction__free(void *ptr);
static size_t rb_cRubython_PyFunction__memsize(const void *ptr);

extern const rb_data_type_t rb_Rubython_PyFunction_type;

VALUE rb_cRubython_PyFunction__wrap(void *obj);

void Init_PyFunction();

#endif // HEADER__RB_PY_FUNCTION_H__INCLUDED
