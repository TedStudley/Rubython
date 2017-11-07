#include "python/Python.h"
#include "ruby.h"

#include "utility.h"
#include "rb_PyObject.h"
#include "rb_PyString.h"
#include "conversion.h"

#define GET_PYOBJECT \
  rb_Rubython_PyObject *py_object; \
  TypedData_Get_Struct(self, rb_Rubython_PyObject, &rb_Rubython_PyObject_type, py_object);

VALUE rb_cPyObject;

static void rb_cRubython_PyObject__mark(void *ptr) {
  rb_Rubython_PyObject *py_object = ptr;
  Py_XINCREF(py_object);
}

static void rb_cRubython_PyObject__free(void *ptr) {
  rb_Rubython_PyObject *py_object = ptr;
  Py_XDECREF(py_object);
}

static size_t rb_cRubython_PyObject__memsize(const void *ptr) {
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
  rb_Rubython_PyObject *py_object = ptr;
  VALUE rb_obj = TypedData_Wrap_Struct(rb_cPyObject, &rb_Rubython_PyObject_type, py_object);
  return rb_obj;
}

VALUE rb_cRubython_PyObject_to_s(VALUE self) {
  GET_PYOBJECT;
  return rb_str_new2(PyString_AS_STRING(PyObject_Str(py_object)));
}

VALUE rb_cRubython_PyObject_inspect(VALUE self) {
  GET_PYOBJECT;
  return rb_str_new2(PyString_AS_STRING(PyObject_Repr(py_object)));
}

VALUE rb_cRubython_PyObject_getattr(VALUE self, VALUE rb_key) {
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

VALUE rb_cRubython_PyObject_callableq(VALUE self) {
  GET_PYOBJECT;
  return PyCallable_Check(py_object) ? Qtrue : Qfalse;
}

// VALUE rb_cRubython_PyObject_call(VALUE self, VALUE args) {
//   GET_PYOBJECT;
//   if (!PyCallable_Check(py_object))
//     rb_raise(rb_eTypeError, "'%s' object is not callable", Py_TYPE(py_object)->tp_name);
// 
//   PyObject *py_callee = py_object;
//   if (!(PyFunction_Check(py_object) || PyCFunction_Check(py_object))) {
//     if (PyObject_HasAttrString(py_object, "__call__"))
//       py_callee = PyObject_GetAttrString(py_object, "__call__");
//     else
//       rb_raise(rb_eTypeError, "Callable '%s' object does not have '__call__' attr... Something is very wrong!", Py_TYPE(py_object)->tp_name);
//   }

//   return PY2RB(((PyFunctionObject *)(py_callee))->func_code);
//   // PyObject *result = PyEval_CallObject(py_object, py_args);
//   // Py_DECREF(py_args);
// 
//   // return PY2RB(result);
// }

void Init_py_object() {
  rb_cPyObject = rb_define_class_under(rb_mPyTypes, "PyObject", rb_cData);
  rb_undef_alloc_func(rb_cPyObject);

  rb_define_method(rb_cPyObject, "to_s", rb_cRubython_PyObject_to_s, 0);
  rb_define_method(rb_cPyObject, "inspect", rb_cRubython_PyObject_inspect, 0);

  rb_define_method(rb_cPyObject, "getattr", rb_cRubython_PyObject_getattr, 1);

  // rb_define_method(rb_cPyObject, "callable?", rb_cRubython_PyObject_callableq, 0);
  // rb_define_method(rb_cPyObject, "call", rb_cRubython_PyObject_call, -2);
}