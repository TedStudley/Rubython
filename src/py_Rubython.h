#ifndef HEADER__PY_RUBYTHON_H__INCLUDED
#define HEADER__PY_RUBYTHON_H__INCLUDED

#include "Python.h"
#include "ruby.h"

extern PyObject *py_mRubython;

static void **PyRubython_API;

PyMODINIT_FUNC initrubython_ext(void);

#endif // HEADER__PY_RUBYTHON_H__INCLUDED
