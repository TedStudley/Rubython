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
  rb_Rubython_PyDict *py_dict = ptr;
  Py_XINCREF(py_dict);
}

static void rb_cRubython_PyDict__free(void *ptr) {
  DEBUG_MARKER;
  rb_Rubython_PyDict *py_dict = ptr;
  Py_XDECREF(py_dict);
}

static size_t rb_cRubython_PyDict__memsize(const void *ptr) {
  DEBUG_MARKER;
  const rb_Rubython_PyDict *py_dict = ptr;
  return sizeof(py_dict) + Py_SIZE(py_dict);
}

const rb_data_type_t rb_Rubython_PyDict_type = {
  "Rubython/PyDict",
  {rb_cRubython_PyDict__mark, rb_cRubython_PyDict__free, rb_cRubython_PyDict__memsize,},
  &rb_Rubython_PyObject_type, 0,
  RUBY_TYPED_FREE_IMMEDIATELY
};

VALUE rb_cRubython_PyDict__wrap(void *ptr) {
  DEBUG_MARKER;
  rb_Rubython_PyDict *py_dict = ptr;
  return TypedData_Wrap_Struct(rb_cPyDict, &rb_Rubython_PyDict_type, py_dict);
}

VALUE
rb_cRubython_PyDict_to_hash(VALUE vobj) {
  GET_PYDICT;
  PyObject *py_dictitems, *py_dictitem,
           *py_key, *py_value;
  VALUE rb_hash = rb_hash_new();
  int dict_size;

  rb_hash = rb_hash_new();
  py_dictitems = PyDict_Items((PyObject *)(self));
  dict_size = PyDict_Size((PyObject *)self);
  for (int idx = 0; idx < dict_size; ++idx) {
    py_dictitem = PyList_GetItem(py_dictitems, idx);
    py_key = PyTuple_GetItem(py_dictitem, 0);
    py_value = PyTuple_GetItem(py_dictitem, 1);
    rb_hash_aset(rb_hash, PY2RB(py_key), PY2RB(py_value));
  }
  return rb_hash;
}

void Init_PyDict() {
  DEBUG_MARKER;
  rb_cPyDict = rb_define_class_under(rb_mPyTypes, "PyDict", rb_cPyObject);

  rb_define_method(rb_cPyDict, "to_hash", rb_cRubython_PyDict_to_hash, 0);
  rb_define_method(rb_cPyDict, "to_h", rb_cRubython_PyDict_to_hash, 0);
}
