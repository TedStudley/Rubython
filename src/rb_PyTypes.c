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

  Init_PyObject();
  Init_PyType();
  Init_PyFunction();
  Init_PyCFunction();
  Init_PyString();
}
