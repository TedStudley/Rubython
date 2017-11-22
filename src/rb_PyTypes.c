#include "utility.h"
#include "rb_Rubython.h"
#include "rb_PyTypes.h"
#include "rb_PyObject.h"
#include "rb_PyString.h"
#include "rb_PyList.h"
#include "rb_PyDict.h"

VALUE rb_mPyTypes;

void Init_PyTypes() {
  DEBUG_MARKER;
  rb_mPyTypes = rb_define_module_under(rb_mRubython, "PyTypes");

  // TODO:
  //   * Remove unneeded types (no need for PyType if we have a PyObject wrapper),
  //   * Add new type wrappers for Dict/List/etc., with proper de-referencing
  //   * Add support for unwrapping Ruby objects that return from Python in a PyObject 
  Init_PyObject();
  Init_PyString();
  Init_PyList();
  Init_PyDict();
}
