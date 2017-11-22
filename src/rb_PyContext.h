#ifndef HEADER__RB_PY_CONTEXT_H__INCLUDED
#define HEADER__RB_PY_CONTEXT_H__INCLUDED

#include "Python.h"
#include "frameobject.h"
#include "ruby.h"

extern VALUE rb_cPyContext;

typedef struct rb_Rubython_PyContextStruct {
  VALUE filename;
  PyFrameObject *py_frame;
} rb_Rubython_PyContext;

static void rb_cRubython_PyContext__mark(void *ptr);
static void rb_cRubython_PyContext__free(void *ptr);
static size_t rb_cRubython_PyContext__memsize(const void *ptr);

static const rb_data_type_t rb_Rubython_PyContext_type;

extern VALUE *pycontext_instance_p;
static VALUE rb_cRubython_PyContext_s_allocate(VALUE klass);

static VALUE rb_cRubython_PyContext_s_get_instance(VALUE klass);
static VALUE rb_cRubython_PyContext_s_finalize(VALUE klass);

static VALUE rb_cRubython_PyContext_initialize(int argc, VALUE *argv, VALUE self);

static VALUE rb_cRubython_PyContext_is_validq(VALUE self);
static VALUE rb_cRubython_PyContext_builtins(VALUE self);
static VALUE rb_cRubython_PyContext_global_variables(VALUE self);
static VALUE rb_cRubython_PyContext_local_variables(VALUE self);

static VALUE rb_cRubython_PyContext_method_missing(int argc, VALUE *argv, VALUE self);

static VALUE rb_cRubython_PyContext_py_eval(VALUE self, VALUE str);
static VALUE rb_cRubython_PyContext_py_exec(VALUE self, VALUE str);

void Init_PyContext(void);

#endif // HEADER__RB_PY_CONTEXT_H__INCLUDED
