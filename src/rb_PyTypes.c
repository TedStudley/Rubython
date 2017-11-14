#include "utility.h"
#include "rb_PyTypes.h"
#include "rb_PyObject.h"
#include "rb_PyType.h"
#include "rb_PyFunction.h"
#include "rb_PyCFunction.h"
#include "rb_PyString.h"

VALUE rb_mPyTypes;


void Init_PyTypes() {
  DEBUG_MARKER;
  rb_mPyTypes = rb_define_module_under(rb_mRubython, "PyTypes");

  // TODO:
  //   * Remove unneeded types (no need for PyType if we have a PyObject wrapper),
  //   * Add new type wrappers for Dict/List/etc., with proper de-referencing
  //   * Add support for unwrapping Ruby objects that return from Python in a PyObject 
  Init_PyObject();
  Init_PyType();
  Init_PyFunction();
  Init_PyCFunction();
  Init_PyString();
}
