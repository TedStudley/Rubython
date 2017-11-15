#ifndef HEADER__RB_PY_CONTEXT_H__INCLUDED
#define HEADER__RB_PY_CONTEXT_H__INCLUDED

#include "ruby.h"
#include "Python.h"
#include "frameobject.h"

extern
VALUE rb_mRubython, rb_cPyContext;

typedef struct rb_Rubython_PyContextStruct {
  VALUE filename;
  PyFrameObject *py_frame;
} rb_Rubython_PyContext;

static void rb_cRubython_PyContext__mark(void *ptr);
static void rb_cRubython_PyContext__free(void *ptr);
static size_t rb_cRubython_PyContext__memsize(const void *ptr);

static const rb_data_type_t rb_Rubython_PyContext_type;

static VALUE rb_cRubython_PyContext_initialize(int argc, VALUE *argv, VALUE self);

void Init_PyContext();

#endif // HEADER__RB_PY_CONTEXT_H__INCLUDED
