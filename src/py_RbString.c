#include "Python.h"
#include "ruby.h"

#include "utility.h"
#include "conversion.h"
#include "py_RbTypes.h"
#include "py_RbObject.h"
#include "py_RbString.h"

PyBufferProcs py_cRubython_RbString_tp_as_buffer;

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
  0, // tp_compare
  0, // tp_repr
  0, // tp_as_number
  0, // tp_as_sequence
  0, // tp_as_mapping
  (hashfunc)py_cRubython_RbString_tp_hash, // tp_hash
  0, // tp_call
  (reprfunc)py_cRubython_RbString_tp_str, // tp_str
  0, // tp_getattro
  0, // tp_setattro
  (PyBufferProcs *)&py_cRubython_RbString_tp_as_buffer, // tp_as_buffer
  Py_TPFLAGS_DEFAULT |
    Py_TPFLAGS_HAVE_GETCHARBUFFER, // tp_flags
  "Rubython Ruby String", // tp_doc
  0,
  0,
  (richcmpfunc)py_cRubython_RbString_tp_richcompare, // tp_richcompare
};
#define PyRbString_Check(ob) PyObject_TypeCheck((ob), &py_Rubython_RbString_type)

PyBufferProcs  py_cRubython_RbString_tp_as_buffer = {
  (readbufferproc)py_cRubython_RbString_bf_getreadbuffer, // (readbufferproc) bf_getreadbuffer
  0, // (writebufferproc) bf_getwritebuffer
  (segcountproc)py_cRubython_RbString_bf_getsegcount, // (segcountproc) bf_getsegcount
  (charbufferproc)py_cRubython_RbString_bf_getcharbuffer, // (charbufferproc) bf_getcharbuffer
};

Py_ssize_t py_cRubython_RbString_bf_getreadbuffer(py_Rubython_RbString *self, Py_ssize_t segment, void **ptrptr) {
  DEBUG_MARKER;
  VALUE rb_string = self->as.value;

  if (segment != 0)
    return -1;

  (*ptrptr) = RSTRING_PTR(rb_string);
  return RSTRING_LEN(rb_string);
}

Py_ssize_t py_cRubython_RbString_bf_getsegcount(py_Rubython_RbString *self, Py_ssize_t *lenp) {
  DEBUG_MARKER;
  VALUE rb_string = self->as.value;
  if (lenp)
    (*lenp) = RSTRING_LEN(rb_string);
  return 1;
}

Py_ssize_t py_cRubython_RbString_bf_getcharbuffer(py_Rubython_RbString *self, Py_ssize_t segment, char **ptrptr) {
  DEBUG_MARKER;
  VALUE rb_string = self->as.value;

  if (segment != 0)
    return -1;

  (*ptrptr) = RSTRING_PTR(rb_string);
  return RSTRING_LEN(rb_string);
}

PyObject *
RbString_WRAP(VALUE ptr) {
  DEBUG_MARKER;
  return RbObject_WRAP_BASE((PyTypeObject *)(py_cRbString), ptr);
}

PyObject *
RbString_Wrap(VALUE ptr) {
  DEBUG_MARKER;

  if (!RB_TYPE_P(ptr, RUBY_T_STRING)) {
    PyErr_Format(PyExc_TypeError, "Expected Ruby String type, but got '%s'",
        rb_obj_classname(ptr));
    return NULL;
  }

  return RbString_WRAP(ptr);
}

static  long
py_cRubython_RbString_tp_hash(py_Rubython_RbString *self) {
  DEBUG_MARKER;
  PyObject *py_str = NULL;
  VALUE rb_str = self->as.value;
  long hashval = 0;

  py_str = PyString_FromStringAndSize(RSTRING_PTR(rb_str), RSTRING_LEN(rb_str));
  hashval = PyObject_Hash(py_str);
  Py_DECREF(py_str);

  return hashval;
}

static PyObject *
py_cRubython_RbString_tp_str(py_Rubython_RbString *self) {
  DEBUG_MARKER;
  VALUE rb_str = self->as.value;
  PyObject *str = NULL;

  str = PyString_FromStringAndSize(RSTRING_PTR(rb_str), RSTRING_LEN(rb_str));
  Py_RETURN(str);
}

static PyObject *
py_cRubython_RbString_tp_richcompare(PyObject *self, PyObject *other, int op) {
  DEBUG_MARKER;
  VALUE rb_str = ((py_Rubython_RbString *)(self))->as.value;
  const char *str_a, *str_b;
  Py_ssize_t len_a, len_b;
  Py_ssize_t min_len;
  PyObject *result = NULL;
  int c;

  // We can only compare PyString or RbString wrappers
  if (!((PyString_Check(self) || PyRbString_Check(self)) &&
        (PyString_Check(other) || PyRbString_Check(other)))) {
    result = Py_NotImplemented;
    goto out;
  }

  // If we're comparing the same object, then the comparison is easy
  if (self == other) {
    switch (op) {
    case Py_EQ:case Py_LE: case Py_GE:
      result = Py_True;
      goto out;
    case Py_NE: case Py_LT: case Py_GT:
      result = Py_False;
      goto out;
    }
  }

  str_a = RSTRING_PTR(rb_str);
  len_a = RSTRING_LEN(rb_str);
  if (PyString_Check(other)) {
    PyStringObject *other_str = (PyStringObject *)(other);
    str_b = other_str->ob_sval;
    len_b = Py_SIZE(other_str);
  } else if (PyRbString_Check(other)) {
    VALUE other_str = ((py_Rubython_RbString *)(other))->as.value;
    str_b = RSTRING_PTR(other_str);
    len_b = RSTRING_LEN(other_str);
  } else {
    // How the hell did we get here?
    DEBUG_MSG("WTF?");
    return NULL;
  }

  if (op == Py_EQ) {
    if (len_a == len_b
        && (str_a[0] == str_b[0])
        && (memcmp(str_a, str_b, len_a) == 0)) {
      result = Py_True;
    } else {
      result = Py_False;
    }
    goto out;
  }
  min_len = (len_a < len_b) ? len_a : len_b;
  if (min_len > 0) {
    c = Py_CHARMASK(*str_a) - Py_CHARMASK(*str_b);
    if (c == 0)
      c = memcmp(str_a, str_b, min_len);
  } else
    c = 0;
  if (c == 0)
    c = (len_a < len_b) ? -1 : (len_a > len_b) ? 1 : 0;
  switch (op) {
  case Py_LT: c = c <  0; break;
  case Py_LE: c = c <= 0; break;
  case Py_EQ: assert(0);  break; /* unreachable */
  case Py_NE: c = c != 0; break;
  case Py_GT: c = c >  0; break;
  case Py_GE: c = c >= 0; break;
  default:
    result = Py_NotImplemented;
    goto out;
  }
  result = c ? Py_True : Py_False;
out:
  Py_INCREF(result);
  return result;
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
