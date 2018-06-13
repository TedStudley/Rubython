#include "Python.h"
#include "ruby.h"

#include "utility.h"
#include "conversion.h"
#include "rb_Rubython.h"
#include "rb_error.h"

VALUE rb_eRubythonError;

void
rb_rubython_error(const char *fmt, ...) {
  DEBUG_MARKER;
  va_list args;
  VALUE argv[1];


  va_start(args, fmt);
  argv[0] = rb_vsprintf(fmt, args);
  va_end(args);

  rb_exc_raise(rb_class_new_instance(1, argv, rb_eRubythonError));
}

void
rb_rubython_py_error(VALUE py_error) {
  DEBUG_MARKER;
  VALUE argv[2];

  DEBUG_MARKER;
  argv[0] = rb_funcall(py_error, rb_intern("message"), 0);
  DEBUG_MARKER;
  argv[1] = py_error;
  rb_exc_raise(rb_class_new_instance(2, argv, rb_eRubythonError));
}

void
rb_rubython_py_error_str(VALUE py_error, const char *fmt, ...) {
  DEBUG_MARKER;
  VALUE argv[2];
  va_list args;

  va_start(args, fmt);
  argv[0] = rb_vsprintf(fmt, args);
  va_end(args);

  argv[1] = py_error;
  rb_exc_raise(rb_class_new_instance(2, argv, rb_eRubythonError));
}

VALUE
rb_eRubython_RubythonError_initialize(int argc, VALUE *argv, VALUE self) {
  DEBUG_MARKER;
  VALUE msg, py_err;

  rb_scan_args(argc, argv, "02", &msg, &py_err);
  rb_iv_set(self, "mesg", msg);
  rb_iv_set(self, "bt", Qnil);
  rb_iv_set(self, "py_cause", py_err);

  return self;
}

void Init_RubythonError(void) {
  rb_eRubythonError = rb_define_class_under(rb_mRubython, "RubythonError", rb_eRuntimeError);

  rb_define_method(rb_eRuntimeError, "initialize", rb_eRubython_RubythonError_initialize, -1);
}
