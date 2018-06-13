#include "Python.h"
#include "structmember.h"
#include "ruby.h"

#include "py_Rubython.h"
#include "py_error.h"

PyObject *py_eRubythonError;

void init_Rubython_RubythonError(void) {
  py_eRubythonError = PyErr_NewException("rubython.rubython_ext.RubythonError", NULL, NULL);
  Py_INCREF(py_eRubythonError);
  PyModule_AddObject(py_mRubython, "RubythonError", py_eRubythonError);
}
