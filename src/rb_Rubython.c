#include "ruby.h"

#include "rb_rubython.h"
#include "rb_PyTypes.h"
#include "rb_PyObject.h"
#include "rb_PyType.h"
#include "rb_PyFunction.h"
#include "rb_PyCFunction.h"
#include "rb_PyString.h"
#include "rb_PyContext.h"

VALUE rb_mRubython;

static PyThreadState *mainThreadState = NULL;

static VALUE rb_Rubython_s_initialize(int argc, VALUE *argv, VALUE klass) {
  ASSERT_PY_NOT_INITIALIZED;
  VALUE retval;

  retval = Qnil;
  Py_Initialize();
  PyEval_InitThreads();
  mainThreadState = PyThreadState_Get();

  if (argc >= 1)
    retval = rb_funcallv(rb_cPyContext, rb_intern("new"), argc, argv);

  return retval;
}

static VALUE rb_Rubython_s_finalize(VALUE klass) {
  ASSERT_PY_INITIALIZED;

  Py_Finalize();
  mainThreadState = NULL;
  return Qnil;
}

void Init_rubython_ext() {
  rb_mRubython = rb_define_module("Rubython");

  rb_define_module_function(rb_mRubython, "initialize!", rb_Rubython_s_initialize, -1);
  rb_define_module_function(rb_mRubython, "finalize!", rb_Rubython_s_finalize, 0);

  Init_py_types();

  Init_py_object();
  Init_py_type();

  Init_py_function();
  Init_py_c_function();

  Init_py_string();

  Init_py_context();
}
