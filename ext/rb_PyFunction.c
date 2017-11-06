#include "python/Python.h"
#include "ruby.h"

#include "conversion.h"
#include "utility.h"
#include "rb_PyObject.h"
#include "rb_PyFunction.h"

VALUE rb_cPyFunction;

static void rb_cRubython_PyFunction__mark(void *ptr) {
  rb_Rubython_PyFunction *py_func = ptr;
  Py_XINCREF(py_func);
}

static void rb_cRubython_PyFunction__free(void *ptr) {
  rb_Rubython_PyFunction *py_func = ptr;
  Py_XDECREF(py_func);
}

static size_t rb_cRubython_PyFunction__memsize(const void *ptr) {
  const rb_Rubython_PyFunction *py_func = ptr;
  return sizeof(py_func) + Py_SIZE(py_func);
}

const rb_data_type_t rb_Rubython_PyFunction_type = {
  "Rubython/PyFunction",
  {rb_cRubython_PyFunction__mark, rb_cRubython_PyFunction__free, rb_cRubython_PyFunction__memsize,},
  &rb_Rubython_PyObject_type, 0,
  RUBY_TYPED_FREE_IMMEDIATELY,
};

#define GET_PYFUNCTION \
  rb_Rubython_PyFunction *py_func; \
  TypedData_Get_Struct(self, rb_Rubython_PyFunction, &rb_Rubython_PyFunction_type, py_func);

VALUE rb_cRubython_PyFunction__wrap(void *ptr) {
  return TypedData_Wrap_Struct(rb_cPyFunction, &rb_Rubython_PyFunction_type, ptr);
}

void Init_py_function() {
  rb_cPyFunction = rb_define_class_under(rb_mPyTypes, "PyFunction", rb_cPyObject);
}
