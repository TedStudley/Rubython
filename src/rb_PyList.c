#include "ruby.h"
#include "Python.h"
#include "listobject.h"

#include "utility.h"
#include "conversion.h"
#include "rb_PyTypes.h"
#include "rb_PyObject.h"
#include "rb_PyList.h"

VALUE rb_cPyList;

static void
rb_cRubython_PyList__mark(void *ptr) {
  DEBUG_MARKER;
}

static void
rb_cRubython_PyList__free(void *ptr) {
  DEBUG_MARKER;
  Py_DECREF((PyObject *)ptr);
}

static size_t
rb_cRubython_PyObject__memsize(const void *ptr) {
  DEBUG_MARKER;
  const rb_Rubython_PyList *py_list = ptr;

  // TODO: Not sure this actually works properly
  return sizeof(py_list) + Py_SIZE(py_list);
}

const rb_data_type_t rb_Rubython_PyList_type = {
  "Rubython/PyList",
  {rb_cRubython_PyList__mark, rb_cRubython_PyList__free, rb_cRubython_PyObject__memsize,},
  &rb_Rubython_PyObject_type, 0,
  RUBY_TYPED_FREE_IMMEDIATELY
};

VALUE PyList_WRAP(PyObject *ptr) {
  DEBUG_MARKER;

  Py_INCREF(ptr);
  VALUE self = TypedData_Wrap_Struct(rb_cPyList, &rb_Rubython_PyList_type, ptr);
  rb_funcall(self, rb_intern("initialize"), 0);
  return self;
}

static VALUE
rb_cRubython_PyList_to_ary(VALUE self) {
  DEBUG_MARKER;
  return Py2RbRaw_List(PYLIST_OBJPTR(self));
}

static VALUE
rb_cRubython_PyList_aref(VALUE self, VALUE index) {
  DEBUG_MARKER;
  // TODO: Support subarray by start/length or range
  PyObject *py_elem = NULL;

  py_elem = PyList_GetItem(PYLIST_OBJPTR(self), NUM2LONG(index));
  if (py_elem == NULL) {
    HandlePyErrors("Error occurred while referencing PyList");
    assert(0); // If we got here, something is funky.
  }

  return PY2RB(py_elem);
}

static VALUE
rb_cRubython_PyList_aset(VALUE self, VALUE index, VALUE value) {
  DEBUG_MARKER;
  int result;

  if (PyList_SetItem(PYLIST_OBJPTR(self), NUM2LONG(index), RB2PY(value))) {
    HandlePyErrors("Error occurred while assigning to PyList");
    assert(0); // If we got here, something if funky.
  }

  return value;
}

static VALUE
rb_cRubython_PyList_eq(VALUE self, VALUE other) {
  DEBUG_MARKER;
  VALUE rb_self;

  if (self == other)
    return Qtrue;
  if (rb_typeddata_is_kind_of(other, &rb_Rubython_PyList_type))
    return (PyObject_RichCompareBool(PYLIST_OBJPTR(self), PYLIST_OBJPTR(other), Py_EQ) ? Qtrue : Qfalse);

  rb_self = rb_funcall(self, rb_intern("to_ary"), 0);
  return rb_funcall(rb_self, rb_intern("=="), 1, other);
}



void Init_PyList() {
  DEBUG_MARKER;
  rb_cPyList = rb_define_class_under(rb_mPyTypes, "PyList", rb_cPyObject);

  rb_define_method(rb_cPyList, "to_ary", rb_cRubython_PyList_to_ary, 0);
  rb_define_method(rb_cPyList, "to_a", rb_cRubython_PyList_to_ary, 0);
  // TODO: Implement `to_ruby` method with 'deep' flag to recurse

  rb_define_method(rb_cPyList, "[]", rb_cRubython_PyList_aref, 1);
  rb_define_method(rb_cPyList, "[]=", rb_cRubython_PyList_aset, 2);

  rb_define_method(rb_cPyList, "==", rb_cRubython_PyList_eq, 1);
}
