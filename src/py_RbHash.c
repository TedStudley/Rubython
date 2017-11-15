#include "Python.h"
#include "ruby.h"

#include "utility.h"
#include "conversion.h"
#include "py_RbTypes.h"
#include "py_RbHash.h"

static PyMappingMethods py_cRubython_RbHash_as_mapping;

PyObject *py_cRbHash;
PyTypeObject py_Rubython_RbHash_type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "rubython.rubython_ext.rb_types.RbHash",
  sizeof(py_Rubython_RbHash),
  0, // tp_itemsize
  0, // tp_dealloc
  0, // tp_print
  0, // tp_getattr
  0, // tp_setattr
  0, // tp_as_async
  0, // tp_repr
  0, // tp_as_number
  0, // tp_as_sequence
  (PyMappingMethods *)&py_cRubython_RbHash_as_mapping,
  0, // tp_hash
  0, // tp_call
  0, // tp_str
  0, // tp_getattro
  0, // tp_setattro
  0, // tp_as_buffer
  Py_TPFLAGS_DEFAULT, // tp_flags
  "Rubython Ruby Hash", // tp_doc
};

PyObject *
py_cRubython_RbHash_s_wrap(PyTypeObject *type, VALUE obj) {
  DEBUG_MARKER;
  py_Rubython_RbHash *self = NULL;
  self = (py_Rubython_RbHash *)py_cRubython_RbObject_s_wrap(type, obj);

  return (PyObject *)(self);
}

PyObject *
RbHashWrap(VALUE obj) {
  DEBUG_MARKER;
  return py_cRubython_RbHash_s_wrap(&py_Rubython_RbHash_type, obj);
}

Py_ssize_t
py_cRubython_RbHash_mp_length(py_Rubython_RbHash *self) {
  DEBUG_MARKER;
  return RHASH_SIZE(self->hash.as.value);
}

PyObject *
py_cRubython_RbHash_mp_subscript(py_Rubython_RbHash *self, PyObject *key) {
  DEBUG_MARKER;
  VALUE rb_result = Qundef, rb_key = Qundef;
  PyObject *result = NULL;

  if (PyNumber_Check(key)) {
    if (PyFloat_Check(key)) {
      rb_key = PyFloat_AS_DOUBLE(key);
    } else if (PyLong_Check(key)) {
      rb_key = PyLong_AsLong(key);
    } else {
      rb_key = PyNumber_AsSsize_t(key, NULL);
    }
  } else if (PyObject_TypeCheck(key, &py_Rubython_RbObject_type)) {
    rb_key = ((py_Rubython_RbObject *)(key))->as.value;
  } else {
    PyErr_SetString(PyExc_KeyError, "RbHash lookup can only be performed with constants or RbObject instances");
    return NULL;
  }

  rb_result = rb_hash_aref(self->hash.as.value, rb_key);
  result = RB2PY(rb_result);
  Py_RETURN(result);
}

static PyMappingMethods py_cRubython_RbHash_as_mapping = {
  (lenfunc)py_cRubython_RbHash_mp_length, // mp_length
  (binaryfunc)py_cRubython_RbHash_mp_subscript, // mp_subscript
  // TODO: Support this.
  0, // mp_ass_subscript
};

void init_Rubython_RbHash(void) {
  DEBUG_MARKER;
  py_Rubython_RbHash_type.tp_base = (PyTypeObject *)py_cRbObject;
  if (PyType_Ready(&py_Rubython_RbHash_type) < 0) {
    DEBUG_MSG("TODO: Handle errors");
    return;
  }

  py_cRbHash = (PyObject *)&py_Rubython_RbHash_type;
  Py_INCREF(py_cRbHash);
  PyModule_AddObject(py_mRbTypes, "RbHash", py_cRbHash);
}
