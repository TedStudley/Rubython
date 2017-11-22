#include "ruby.h"
#include "Python.h"

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

VALUE PyString_WRAP(PyObject *ptr) {
  DEBUG_MARKER;

  Py_INCREF(ptr);
  VALUE self = TypedData_Wrap_Struct(rb_cPyString, &rb_Rubython_PyString_type, ptr);
  rb_funcall(self, rb_intern("initialize"), 0);
  return self;
}

static st_index_t
rb_cRubython_PyString_hash(PyStringObject *str) {
  DEBUG_MARKER;
  // TODO: Actually support encodings. (Maybe just convert to a Ruby string?)
  return rb_memhash((const void *)PyString_AS_STRING(str), PyString_GET_SIZE(str)) ^ 0;
}

VALUE
rb_cRubython_PyString_hash_m(VALUE self) {
  DEBUG_MARKER;
  return INT2FIX(rb_cRubython_PyString_hash(PYSTRING_PTR(self)));
}

VALUE
rb_cRubython_PyString_eq(VALUE self, VALUE other) {
  DEBUG_MARKER;

  if (self == other)
    return Qtrue;
  if (rb_typeddata_is_kind_of(other, &rb_Rubython_PyString_type))
    return (_PyString_Eq(PYSTRING_OBJPTR(self), PYSTRING_OBJPTR(other)) ? Qtrue : Qfalse);

  VALUE rb_str = Py2RbRaw_String(PYSTRING_OBJPTR(self));
  return rb_funcall(rb_str, rb_intern("=="), 1, other);
}

VALUE rb_cRubython_PyString_to_s(VALUE self) {
  DEBUG_MARKER;

  return Py2RbRaw_String(PYSTRING_OBJPTR(self));
}

VALUE rb_cRubython_PyString_inspect(VALUE self) {
  DEBUG_MARKER;
  return Py2RbRaw_String(PyString_Repr(PYSTRING_OBJPTR(self), 1));
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
