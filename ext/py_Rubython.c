#include "python/Python.h"
#include "ruby.h"
#include "ruby/debug.h"

#include "utility.h"
#include "py_Rubython.h"
#include "py_RbContext.h"
#include "py_RbTypes.h"
#include "py_RbObject.h"
#include "py_RbArray.h"
#include "py_RbHash.h"

PyObject *py_mRubython;

static char module_docstring[] =
    "This module provides a symmetric Ruby/Python embedded interface.";

static PyMethodDef rubythonMethods[] = {
  {NULL}
};

PyMODINIT_FUNC
initrubython_ext(void) {
  DEBUG_MARKER;
  py_mRubython = Py_InitModule("rubython.rubython_ext", rubythonMethods);
  if (py_mRubython == NULL) {
    DEBUG_MSG("TODO: Handle Errors!");
    return;
  }
  Py_INCREF(py_mRubython);

  init_Rubython_RbContext();
  init_Rubython_RbTypes();
}
