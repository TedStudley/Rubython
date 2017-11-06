#include "python/Python.h"

#include "utility.h"
#include "py_Rubython.h"
#include "py_RbTypes.h"
#include "py_RbObject.h"
#include "py_RbArray.h"
#include "py_RbHash.h"

PyObject *py_mRbTypes;

static PyMethodDef py_mRubython_RbTypes_methods[] = {
  {NULL}
};

void init_Rubython_RbTypes(void) {
  DEBUG_MARKER;

  py_mRbTypes = Py_InitModule("rubython.rubython_ext.rb_types", py_mRubython_RbTypes_methods);
  if (py_mRbTypes == NULL) {
    DEBUG_MSG("TODO: Handle Errors!");
    return;
  }
  PyModule_AddObject(py_mRubython, "rb_types", py_mRbTypes);
  Py_INCREF(py_mRbTypes);

  init_Rubython_RbObject();
  init_Rubython_RbArray();
  init_Rubython_RbHash();
}
