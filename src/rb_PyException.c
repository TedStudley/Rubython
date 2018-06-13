#include "Python.h"
#include "ruby.h"

#include "utility.h"
#include "conversion.h"
#include "rb_PyTypes.h"
#include "rb_PyObject.h"
#include "rb_PyException.h"

#define GET_PYEXCEPTION \
  rb_Rubython_PyException *self; \
  TypedData_Get_Struct(vobj, rb_Rubython_PyException, &rb_Rubython_PyException_type, self);

VALUE rb_cPyException;

static void
rb_cRubython_PyException__mark(void *ptr) {
  DEBUG_MARKER;
}

static void
rb_cRubython_PyException__free(void *ptr) {
  DEBUG_MARKER;
  const rb_Rubython_PyException *py_exception = ptr;
  Py_XDECREF(py_exception);
}

static size_t
rb_cRubython_PyException__memsize(const void *ptr) {
  DEBUG_MARKER;
  const rb_Rubython_PyException *py_exception = ptr;
  return sizeof(py_exception) + Py_SIZE(py_exception);
}

const rb_data_type_t rb_Rubython_PyException_type = {
  "Rubython/PyException",
  {rb_cRubython_PyException__mark, rb_cRubython_PyException__free, rb_cRubython_PyException__memsize,},
  &rb_Rubython_PyObject_type, 0,
  RUBY_TYPED_FREE_IMMEDIATELY
};

VALUE PyExceptionWrap(rb_Rubython_PyException *obj) {
  DEBUG_MARKER;
  VALUE self = TypedData_Wrap_Struct(rb_cPyException, &rb_Rubython_PyException_type, obj);
  rb_funcall(self, rb_intern("initialize"), 0);
  return self;
}

static VALUE
rb_cRubython_PyException_message(VALUE vobj) {
  DEBUG_MARKER;
  GET_PYEXCEPTION;
  return Py2RbRaw_String(self->message);
}

void Init_PyException(void) {
  DEBUG_MARKER;
  rb_cPyException = rb_define_class_under(rb_mPyTypes, "PyException", rb_cPyObject);

  rb_define_method(rb_cPyException, "message", rb_cRubython_PyException_message, 0);
}
