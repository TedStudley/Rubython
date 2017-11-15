#include "Python.h"
#include "ruby.h"

#include "utility.h"
#include "conversion.h"
#include "py_RbTypes.h"
#include "py_RbObject.h"
#include "py_RbString.h"

// static PyBufferProcs py_cRubython_RbString_tp_as_buffer;

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
  (reprfunc)py_cRubython_RbString_str, // tp_str
  0, // tp_getattro
  0, // tp_setattro
  (PyBufferProcs *)&py_cRubython_RbString_tp_as_buffer, // tp_as_buffer
  Py_TPFLAGS_DEFAULT |
    Py_TPFLAGS_HAVE_GETCHARBUFFER, // tp_flags
  "Rubython Ruby String", // tp_doc
};

static PyBufferProcs  py_cRubython_RbString_tp_as_buffer = {
  (readbufferproc)py_cRubython_RbString_bf_getreadbuffer, // (readbufferproc) bf_getreadbuffer
  0, // (writebufferproc) bf_getwritebuffer
  (segcountproc)py_cRubython_RbString_bf_getsegcount, // (segcountproc) bf_getsegcount
  (charbufferproc)py_cRubython_RbString_bf_getcharbuffer, // (charbufferproc) bf_getcharbuffer
};

Py_ssize_t py_cRubython_RbString_bf_getreadbuffer(py_Rubython_RbString *self, Py_ssize_t segment, void **ptrptr) {
  DEBUG_MARKER;
  VALUE rb_string = self->string.as.value;

  if (segment != 0)
    return -1;

  (*ptrptr) = RSTRING_PTR(rb_string);
  return RSTRING_LEN(rb_string);
}

Py_ssize_t py_cRubython_RbString_bf_getsegcount(py_Rubython_RbString *self, Py_ssize_t *lenp) {
  DEBUG_MARKER;
  VALUE rb_string = self->string.as.value;
  if (lenp)
    (*lenp) = RSTRING_LEN(rb_string);
  return 1;
}

Py_ssize_t py_cRubython_RbString_bf_getcharbuffer(py_Rubython_RbString *self, Py_ssize_t segment, char **ptrptr) {
  DEBUG_MARKER;
  VALUE rb_string = self->string.as.value;

  if (segment != 0)
    return -1;

  (*ptrptr) = RSTRING_PTR(rb_string);
  return RSTRING_LEN(rb_string);
}

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

static PyObject *
py_cRubython_RbString_str(py_Rubython_RbString *self) {
  DEBUG_MARKER;
  VALUE rb_str = self->string.as.value;
  PyObject *str = NULL;

  str = PyString_FromStringAndSize(RSTRING_PTR(rb_str), RSTRING_LEN(rb_str));
  Py_RETURN(str);
}

void init_Rubython_RbString(void) {
  DEBUG_MARKER;
  py_Rubython_RbString_type.tp_base = (PyTypeObject *)py_cRbObject;
  if (PyType_Ready(&py_Rubython_RbString_type) < 0) {
    DEBUG_MSG("TODO: Handle errors");
    return;
  }

  py_cRbString = (PyObject *)&py_Rubython_RbString_type;
  Py_INCREF(py_cRbString);
  PyModule_AddObject(py_mRbTypes, "RbString", py_cRbString);
}
