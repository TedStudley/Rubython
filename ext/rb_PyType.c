#include "ruby.h"
#include "python/Python.h"
#include "python/object.h"

#include "rb_PyObject.h"
#include "rb_PyType.h"

#define GET_PYTYPE \
  rb_Rubython_PyType *py_type; \
  TypedData_Get_Struct(self, rb_Rubython_PyType, &rb_Rubython_PyType_type, py_type);

VALUE rb_cPyType;

static void rb_cRubython_PyType__mark(void *ptr) {
  rb_Rubython_PyType *py_type = ptr;
  Py_XINCREF(py_type);
}

static void rb_cRubython_PyType__free(void *ptr) {
  rb_Rubython_PyType *py_type = ptr;
  Py_XDECREF(py_type);
}

static size_t rb_cRubython_PyType__memsize(const void *ptr) {
  const rb_Rubython_PyType *py_type = ptr;
  return sizeof(py_type) + Py_SIZE(py_type);
}

const rb_data_type_t rb_Rubython_PyType_type = {
  "Rubython/PyType",
  {rb_cRubython_PyType__mark, rb_cRubython_PyType__free, rb_cRubython_PyType__memsize,},
  &rb_Rubython_PyObject_type, 0,
  RUBY_TYPED_FREE_IMMEDIATELY
};

VALUE rb_cRubython_PyType__wrap(void *ptr) {
  rb_Rubython_PyType *py_type = ptr;
  return TypedData_Wrap_Struct(rb_cPyType, &rb_Rubython_PyType_type, py_type);
}

void Init_py_type() {
  rb_cPyType = rb_define_class_under(rb_mPyTypes, "PyType", rb_cPyObject);

  rb_define_method(rb_cPyType, "inspect", rb_cRubython_PyObject_to_s, 0);
}
