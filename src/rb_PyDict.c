#include "ruby.h"
#include "Python.h"
#include "dictobject.h"

#include "utility.h"
#include "conversion.h"
#include "rb_PyTypes.h"
#include "rb_PyObject.h"
#include "rb_PyDict.h"

#define GET_PYDICT \
  rb_Rubython_PyDict *self; \
  TypedData_Get_Struct(vobj, rb_Rubython_PyDict, &rb_Rubython_PyDict_type, self);

VALUE rb_cPyDict;

static void rb_cRubython_PyDict__mark(void *ptr) {
  DEBUG_MARKER;
}

static void rb_cRubython_PyDict__free(void *ptr) {
  DEBUG_MARKER;
  Py_DECREF(ptr);
}

static size_t rb_cRubython_PyDict__memsize(const void *ptr) {
  DEBUG_MARKER;
  const rb_Rubython_PyDict *py_dict = ptr;

  // TODO: I don't think this actually works properly
  return sizeof(py_dict) + Py_SIZE(py_dict);
}

const rb_data_type_t rb_Rubython_PyDict_type = {
  "Rubython/PyDict",
  {rb_cRubython_PyDict__mark, rb_cRubython_PyDict__free, rb_cRubython_PyDict__memsize,},
  &rb_Rubython_PyObject_type, 0,
  RUBY_TYPED_FREE_IMMEDIATELY
};

VALUE PyDict_WRAP(PyObject *ptr) {
  DEBUG_MARKER;

  Py_INCREF(ptr);
  VALUE self = TypedData_Wrap_Struct(rb_cPyDict, &rb_Rubython_PyDict_type, ptr);
  rb_funcall(self, rb_intern("initialize"), 0);
  return self;
}

static VALUE
rb_cRubython_PyDict_to_hash(VALUE self) {
  DEBUG_MARKER;
  return Py2RbRaw_Dict(PYDICT_OBJPTR(self));
}

static VALUE
rb_cRubython_PyDict_aref(VALUE self, VALUE key) {
  DEBUG_MARKER;
  PyObject *py_value = NULL;

  py_value = PyDict_GetItem(PYDICT_OBJPTR(self), RB2PY(key));
  if (py_value == NULL) {
    HandlePyErrors("Error occurred while grabbing from PyDict");
    // If we get here, then there just wasn't a value for that key
    return Qnil;
  }

  return PY2RB(py_value);
}

static VALUE
rb_cRubython_PyDict_aset(VALUE self, VALUE key, VALUE value) {
  DEBUG_MARKER;

  if (PyDict_SetItem(PYDICT_OBJPTR(self), RB2PY(key), RB2PY(value))) {
    HandlePyErrors("Error occurring when storing in PyDict");
    assert(0); // If we got here, something is funky.
  }

  return value;
}

void Init_PyDict(void) {
  DEBUG_MARKER;
  rb_cPyDict = rb_define_class_under(rb_mPyTypes, "PyDict", rb_cPyObject);

  rb_define_method(rb_cPyDict, "to_hash", rb_cRubython_PyDict_to_hash, 0);
  rb_define_method(rb_cPyDict, "to_h", rb_cRubython_PyDict_to_hash, 0);
  // TODO: Support deep/shallow 'to_ruby' method

  rb_define_method(rb_cPyDict, "[]", rb_cRubython_PyDict_aref, 1);
  rb_define_method(rb_cPyDict, "[]=", rb_cRubython_PyDict_aset, 2);
}
