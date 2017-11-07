#include "python/Python.h"
#include "ruby/ruby.h"

#include "utility.h"
#include "conversion.h"
// #include "py_Rubython.h"
#include "py_RbTypes.h"
#include "py_RbArray.h"

// static PyMethodDef py_cRubython_RbArray_methods[];
// static PyMemberDef py_cRubython_RbArray_members[];
static PySequenceMethods py_cRubython_RbArray_as_sequence;

PyObject *py_cRbArray;

PyTypeObject py_Rubython_RbArray_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "rubython.RbArray",
    sizeof(py_Rubython_RbArray),
    0, // tp_itemsize
    0, // tp_dealloc
    0, // tp_print
    0, // tp_getattr
    0, // tp_setattr
    0, // tp_as_async
    0, // tp_repr
    0, // tp_as_number
    (PySequenceMethods *)&py_cRubython_RbArray_as_sequence, // tp_as_sequence
    0, // tp_as_mapping
    0, // tp_hash
    0, // tp_call
    0, // tp_str
    0, // tp_getattro
    0, // tp_setattro
    0, // tp_as_buffer
    Py_TPFLAGS_DEFAULT, // tp_flags
    "Rubython Ruby Array", // tp_doc
};

PyObject *
py_cRubython_RbArray_s_wrap(PyTypeObject *type, VALUE rb_object) {
  DEBUG_MARKER;
  py_Rubython_RbArray *self = NULL;
  self = (py_Rubython_RbArray *)py_cRubython_RbObject_s_wrap(type, rb_object);

  Py_RETURN((PyObject *)(self));
}
PyObject *
RbArrayWrap(VALUE obj) {
  DEBUG_MARKER;
  return py_cRubython_RbArray_s_wrap(&py_Rubython_RbArray_type, obj);
}

Py_ssize_t
py_cRubython_RbArray_sq_length(py_Rubython_RbArray *self) {
  DEBUG_MARKER;
  return RARRAY_LEN(self->array.as.rb_array);
}

PyObject *
py_cRubython_RbArray_sq_item(py_Rubython_RbArray *self, Py_ssize_t idx) {
  DEBUG_MARKER;
  PyObject *result = NULL;

  if (idx >= RARRAY_LEN(self->array.as.rb_array)) {
    PyErr_SetString(PyExc_IndexError, "RbArray index out of range");
    return NULL;
  }
  result = RB2PY(RARRAY_AREF(self->array.as.rb_array, idx));
  Py_RETURN(result);
}

static PySequenceMethods py_cRubython_RbArray_as_sequence = {
  (lenfunc)py_cRubython_RbArray_sq_length, // sq_length
  0, // sq_concat
  0, // sq_repeat
  (ssizeargfunc)py_cRubython_RbArray_sq_item, // sq_item
  0, // sq_ass_item
  0, // sq_contains
  0, // sq_inplace_concat
  0, // sq_inplace_repeat
};

void init_Rubython_RbArray(void) {
  DEBUG_MARKER;
  py_Rubython_RbArray_type.tp_base = (PyTypeObject *)py_cRbObject;
  if (PyType_Ready(&py_Rubython_RbArray_type) < 0) {
    // TODO: actual errors
    fprintf(stderr, "Something went wrong while attempting to initialize RbArray");
    return;
  }

  py_cRbArray = (PyObject *)&py_Rubython_RbArray_type;
  Py_INCREF(py_cRbArray);
  PyModule_AddObject(py_mRbTypes, "RbArray", py_cRbArray);
}
