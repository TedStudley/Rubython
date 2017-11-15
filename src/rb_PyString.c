#include "Python.h"
#include "ruby.h"

#include "utility.h"
#include "conversion.h"
#include "rb_PyTypes.h"
#include "rb_PyObject.h"
#include "rb_PyString.h"

VALUE rb_cPyString;

static void rb_cRubython_PyString__mark(void *ptr) {
  DEBUG_MARKER;
  rb_Rubython_PyString *py_string = ptr;
  Py_XINCREF(py_string);
}

static void rb_cRubython_PyString__free (void *ptr) {
  DEBUG_MARKER;
  rb_Rubython_PyString *py_string = ptr;
  Py_XDECREF(py_string);
}

static size_t rb_cRubython_PyString__memsize(const void *ptr) {
  DEBUG_MARKER;
  const rb_Rubython_PyString *py_string = ptr;
  return sizeof(py_string) + sizeof(py_string->ob_sval);
}

const rb_data_type_t rb_Rubython_PyString_type = {
  "Rubython/PyString",
  {rb_cRubython_PyString__mark, rb_cRubython_PyString__free, rb_cRubython_PyString__memsize,},
  &rb_Rubython_PyObject_type, 0,
  RUBY_TYPED_FREE_IMMEDIATELY,
};

#define GET_PYSTRING \
  rb_Rubython_PyString *py_string; \
  TypedData_Get_Struct(self, rb_Rubython_PyString, &rb_Rubython_PyString_type, py_string);

VALUE rb_cRubython_PyString__wrap(void *ptr) {
  DEBUG_MARKER;
  rb_Rubython_PyString *py_string = ptr;
  VALUE rb_obj = TypedData_Wrap_Struct(rb_cPyString, &rb_Rubython_PyString_type, py_string);
  return rb_obj;
}

static st_index_t
rb_cRubython_PyString_hash(PyObject *py_string) {
  DEBUG_MARKER;
  return rb_memhash((const void *)PyString_AsString(py_string), PyString_Size(py_string)) ^ 0;
}

VALUE
rb_cRubython_PyString_hash_m(VALUE self) {
  DEBUG_MARKER;
  GET_PYSTRING;
  st_index_t hval = rb_cRubython_PyString_hash((PyObject *)(py_string));
  return INT2FIX(hval);
}

VALUE
rb_cRubython_PyString_eq(VALUE self, VALUE other) {
  DEBUG_MARKER;
  GET_PYSTRING;
  VALUE rb_str;

  if (self == other)
    return Qtrue;

  if (rb_typeddata_is_kind_of(other, &rb_Rubython_PyString_type)) {
    PyStringObject *py_other;
    TypedData_Get_Struct(self, rb_Rubython_PyString, &rb_Rubython_PyString_type, py_other);
    return (_PyString_Eq((PyObject *)(py_string), (PyObject *)(py_other)) ? Qtrue : Qfalse);
  }

  rb_str = rb_str_new2(PyString_AS_STRING(py_string));
  return rb_funcall(rb_str, rb_intern("=="), 1, other);
}

VALUE rb_cRubython_PyString_to_s(VALUE self) {
  DEBUG_MARKER;
  GET_PYSTRING;
  return rb_str_new2(PyString_AS_STRING(py_string));
}

VALUE rb_cRubython_PyString_inspect(VALUE self) {
  DEBUG_MARKER;
  GET_PYSTRING;
  return rb_str_new2(PyString_AS_STRING(PyString_Repr((PyObject *)(py_string), 1)));
}

void Init_PyString() {
  DEBUG_MARKER;
  rb_cPyString = rb_define_class_under(rb_mPyTypes, "PyString", rb_cPyObject);

  rb_define_method(rb_cPyString, "hash", rb_cRubython_PyString_hash_m, 0);

  rb_define_method(rb_cPyString, "==", rb_cRubython_PyString_eq, 1);
  rb_define_method(rb_cPyString, "eql?", rb_cRubython_PyString_eq, 1);

  rb_define_method(rb_cPyString, "to_str", rb_cRubython_PyString_to_s, 0);
  rb_define_method(rb_cPyString, "to_s", rb_cRubython_PyString_to_s, 0);
  rb_define_method(rb_cPyString, "inspect", rb_cRubython_PyString_inspect, 0);
}
