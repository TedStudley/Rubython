#include "Python.h"
#include "ruby.h"

#include "utility.h"
#include "conversion.h"
#include "rb_PyTypes.h"
#include "rb_PyObject.h"
#include "rb_PyCFunction.h"

VALUE rb_cPyCFunction;

static void rb_cRubython_PyCFunction__mark(void *ptr) {
  DEBUG_MARKER;
  rb_Rubython_PyCFunction *py_cfunc = ptr;
  Py_XINCREF(py_cfunc);
}

static void rb_cRubython_PyCFunction__free(void *ptr) {
  DEBUG_MARKER;
  rb_Rubython_PyCFunction *py_cfunc = ptr;
  Py_XDECREF(py_cfunc);
}

static size_t rb_cRubython_PyCFunction__memsize(const void *ptr) {
  DEBUG_MARKER;
  const rb_Rubython_PyCFunction *py_cfunc = ptr;
  return sizeof(py_cfunc) + Py_SIZE(py_cfunc);
}

const rb_data_type_t rb_Rubython_PyCFunction_type = {
  "Rubython/PyCFunction",
  {rb_cRubython_PyCFunction__mark, rb_cRubython_PyCFunction__free, rb_cRubython_PyCFunction__memsize,},
  &rb_Rubython_PyObject_type, 0,
  RUBY_TYPED_FREE_IMMEDIATELY,
};

#define GET_PYCFUNCTION \
  rb_Rubython_PyCFunction *py_cfunc; \
  TypedData_Get_Struct(self, rb_Rubython_PyCFunction, &rb_Rubython_PyCFunction_type, py_cfunc);

VALUE rb_cRubython_PyCFunction__wrap(void *ptr) {
  DEBUG_MARKER;
  return TypedData_Wrap_Struct(rb_cPyCFunction, &rb_Rubython_PyCFunction_type, ptr);
}

VALUE rb_cRubython_PyCFunction_call(VALUE self, VALUE args) {
  DEBUG_MARKER;
  GET_PYCFUNCTION;
  VALUE rb_kwargs, rb_args;
  PyObject *py_args, *py_kwargs;

  rb_args = rb_ary_subseq(args, -1, 1);
  rb_kwargs = rb_ary_entry(args, -1);

  if (rb_args == Qnil)
    rb_args = rb_ary_new();

  py_args = Rb2Py_Tuple(rb_args);
  py_kwargs = RB2PY(rb_kwargs);

  PyObject *py_result = Py_None; // PyCFunction_Call((PyObject *)(py_cfunc), py_args, py_kwargs);
  Py_XDECREF(py_args); Py_XDECREF(py_kwargs);
  return PY2RB(py_result); // PY2RB(py_result);
}

void Init_PyCFunction() {
  DEBUG_MARKER;
  rb_cPyCFunction = rb_define_class_under(rb_mPyTypes, "PyCFunction", rb_cPyObject);

  rb_define_method(rb_cPyCFunction, "call", rb_cRubython_PyCFunction_call, -2);
}
