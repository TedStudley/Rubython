#include "python/Python.h"
#include "ruby/ruby.h"

#include "utility.h"
#include "conversion.h"
#include "py_RbTypes.h"
#include "py_RbArray.h"

PyObject *py_cRbString;

PyTypeObject py_Rubython_RbString_type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "rubython.rubython_ext.rb_types.RbString",
  sizeof(py_Rubython_RbString),
  0, // tp_itemsize
  0, // tp_dealloc
  0, // tp_print
  0, // tp_getattr
  0, // tp_setattr
  0, // tp_as_async
  0, // tp_repr
  0, // tp_as_number
  0, // tp_as_sequence
  0, // tp_as_mapping
  0, // tp_hash
  0, // tp_call
  0, // TODO: tp_str
  0, // tp_getattro
  0, // tp_setattro
  0, // tp_as_buffer
  Py_TPFLAGS_DEFAULT, // tp_flags
  "Rubython Ruby String", // tp_doc
};

PyObject *
py_cRubython_RbString_s_wrap(PyTypeObject *type, VALUE rb_object) {
  DEBUG_MARKER;
  py_Rubython_RbString *self = NULL;
  self = (py_Rubython_RbString *)py_cRubython_RbObject_s_wrap(type, rb_object);

  Py_RETURN((PyObject *)(self));
}
PyObject *
RbStringWrap(VALUE obj) {
  DEBUG_MARKER;
  return py_cRubython_RbObject_s_wrap(&py_Rubython_RbString_type, obj);
}

void init_Rubython_RbString(void) {
  DEBUG_MARKER;
  py_Rubython_RbString_type.tp_base = (PyTypeObject *)py_cRbObject;
  if (PyType_Ready(&py_Rubython_RbString_type) < 0) {
    // TODO: Actual errors
    DEBUG_MSG("TODO: Handle errors");
    return;
  }

  py_cRbString = (PyObject *)&py_Rubython_RbString_type;
  Py_INCREF(py_cRbString);
  PyModule_AddObject(py_mRbTypes, "RbString", py_cRbString);
}
