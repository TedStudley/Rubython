#ifndef HEADER__RB_PY_EXCEPTION_H__INCLUDED
#define HEADER__RB_PY_EXCEPTION_H__INCLUDED

#include "Python.h"
#include "ruby.h"

extern VALUE rb_cPyException;

typedef PyBaseExceptionObject rb_Rubython_PyException;

static void rb_cRubython_PyException__mark(void *ptr);
static void rb_cRubython_PyException__free(void *ptr);
static size_t rb_cRubython_PyException__memsize(const void *ptr);

extern const rb_data_type_t rb_Rubython_PyException_type;

VALUE PyExceptionWrap(rb_Rubython_PyException *obj);

void Init_PyException(void);

#endif // HEADER__RB_PY_EXCEPTION_H__INCLUDED
