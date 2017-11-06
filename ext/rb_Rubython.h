#ifndef HEADER__RB_RUBYTHON_H__INCLUDED
#define HEADER__RB_RUBYTHON_H__INCLUDED

#include "Python/python.h"
#include "ruby.h"

#define ASSERT_PY_NOT_INITIALIZED \
  if (Py_IsInitialized()) \
    rb_raise(rb_eRuntimeError, "Rubython is already initialized!");

#define ASSERT_PY_INITIALIZED \
  if (!Py_IsInitialized()) \
    rb_raise(rb_eRuntimeError, "Rubython is not initialized yet!");

extern VALUE rb_mRubython;

void Init_rubython_ext(void);

#endif // HEADER__RUBYTHON_H__INCLUDED
