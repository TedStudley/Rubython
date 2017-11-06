#include "rb_PyTypes.h"

VALUE rb_mPyTypes;

void Init_py_types() {
  rb_mPyTypes = rb_define_module_under(rb_mRubython, "PyTypes");
}
