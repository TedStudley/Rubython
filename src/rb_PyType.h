#ifndef HEADER__RB_PYTYPE_H__INCLUDED
#define HEADER__RB_PYTYPE_H__INCLUDED

#include "ruby.h"
#include "python/Python.h"
#include "python/object.h"

extern VALUE rb_mPyTypes, rb_cPyType;

typedef PyTypeObject rb_Rubython_PyType;

static void rb_cRubython_PyType__mark(void *ptr);
static void rb_cRubython_PyType__free(void *ptr);
static size_t rb_cRubython_PyType__memsize(const void *ptr);

extern const rb_data_type_t rb_Rubython_PyType_type;

VALUE rb_cRubython_PyType__wrap(void *ptr);

void Init_PyType();

#endif // HEADER__RB_PYTYPE_H__INCLUDED
