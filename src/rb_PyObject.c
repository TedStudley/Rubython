#include "python/Python.h"
#include "ruby.h"

#include "utility.h"
#include "conversion.h"
#include "rb_PyTypes.h"
#include "rb_PyObject.h"
#include "rb_PyString.h"

#define GET_PYOBJECT \
  rb_Rubython_PyObject *py_object; \
  TypedData_Get_Struct(self, rb_Rubython_PyObject, &rb_Rubython_PyObject_type, py_object);

VALUE rb_cPyObject;

static void rb_cRubython_PyObject__mark(void *ptr) {
  DEBUG_MARKER;
  rb_Rubython_PyObject *py_object = ptr;
  Py_XINCREF(py_object);
}

static void rb_cRubython_PyObject__free(void *ptr) {
  DEBUG_MARKER;
  rb_Rubython_PyObject *py_object = ptr;
  Py_XDECREF(py_object);
}

static size_t rb_cRubython_PyObject__memsize(const void *ptr) {
  DEBUG_MARKER;
  const rb_Rubython_PyObject *py_object = ptr;
  return sizeof(py_object) + Py_SIZE(py_object);
}

const rb_data_type_t rb_Rubython_PyObject_type = {
  "Rubython/PyObject",
  {rb_cRubython_PyObject__mark, rb_cRubython_PyObject__free, rb_cRubython_PyObject__memsize,},
  0, 0,
  RUBY_TYPED_FREE_IMMEDIATELY
};

VALUE rb_cRubython_PyObject__wrap(void *ptr) {
  DEBUG_MARKER;
  rb_Rubython_PyObject *py_object = ptr;
  VALUE rb_obj = TypedData_Wrap_Struct(rb_cPyObject, &rb_Rubython_PyObject_type, py_object);
  return rb_obj;
}

VALUE
rb_cRubython_PyObject_to_s(VALUE self) {
  DEBUG_MARKER;
  GET_PYOBJECT;
  return rb_str_new2(PyString_AS_STRING(PyObject_Str(py_object)));
}

VALUE
rb_cRubython_PyObject_inspect(VALUE self) {
  DEBUG_MARKER;
  GET_PYOBJECT;
  return rb_str_new2(PyString_AS_STRING(PyObject_Repr(py_object)));
}

VALUE
rb_cRubython_PyObject_getattr(VALUE self, VALUE rb_key) {
  DEBUG_MARKER;
  GET_PYOBJECT;
  const char *key;
  if (RB_TYPE_P(rb_key, T_STRING)) {
    key = RSTRING_PTR(rb_key);
  } else if (SYMBOL_P(rb_key)) {
    key = rb_id2name(SYM2ID(rb_key));
  } else
    rb_raise(rb_eTypeError, "Expected string or symbol");

  if (PyObject_HasAttrString(py_object, key)) {
    return PY2RB(PyObject_GetAttrString(py_object, key));
  } else
    rb_raise(rb_eKeyError, "object has no attribute '%s'", key);
}

VALUE
rb_cRubython_PyObject_callableq(VALUE self) {
  DEBUG_MARKER;
  GET_PYOBJECT;
  return PyCallable_Check(py_object) ? Qtrue : Qfalse;
}

VALUE
rb_cRubython_PyObject_call(VALUE self, VALUE args) {
  DEBUG_MARKER;
  GET_PYOBJECT;
  PyObject *py_result, *py_args;
  VALUE result = Qundef;

  convertRbArgs(args, &py_args);
  // TODO: Support kwargs
  py_result = PyObject_Call(py_object, py_args, NULL);
  if (py_result == NULL) {
    DEBUG_MSG("TODO: Handle Errors!");
    rb_raise(rb_eTypeError, "'<type>' object is not callable");
  }

  return PY2RB(py_result);
}

void Init_PyObject() {
  DEBUG_MARKER;

  // TODO:
  //   * Formalize what should be accessible via method_missing -- If we call
  //     `obj.foo`, should we get back the method instance or the result of
  //     calling `obj.foo()`?
  //
  //     Maybe this can depend upon whether the object is a callable/whether
  //     arguments were passed. If no arguments are passed, the default behavior
  //     could be to return the method instance. If the the method is called with
  //     an '!' appended, then it should be called even with no arguments. If
  //     arguments are passed, then the method should be called with those
  //     arguments. This _should_ cover pretty much every base.
  //
  //   * Properly support calling methods with arguments from Ruby.

  rb_cPyObject = rb_define_class_under(rb_mPyTypes, "PyObject", rb_cData);
  rb_undef_alloc_func(rb_cPyObject);

  rb_define_method(rb_cPyObject, "to_s", rb_cRubython_PyObject_to_s, 0);
  rb_define_method(rb_cPyObject, "inspect", rb_cRubython_PyObject_inspect, 0);

  rb_define_method(rb_cPyObject, "getattr", rb_cRubython_PyObject_getattr, 1);
  rb_define_method(rb_cPyObject, "call", rb_cRubython_PyObject_call, -2);
}
