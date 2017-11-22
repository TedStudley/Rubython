#include "Python.h"
#include "ruby.h"

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
    0, // tp_compare
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
    0, // tp_traverse
    0, // tp_clear
    (richcmpfunc)py_cRubython_RbArray_tp_richcompare, // tp_richcompare
};
#define PyRbArrayObject (&py_Rubython_RbArray_type)
#define PyRbArray_Check(op) ((op)->ob_type == PyRbArrayObject)

static PySequenceMethods py_cRubython_RbArray_as_sequence = {
  (lenfunc)py_cRubython_RbArray_sq_length, // sq_length
  0, // sq_concat
  0, // sq_repeat
  (ssizeargfunc)py_cRubython_RbArray_sq_item, // sq_item
  0, // sq_slice
  (ssizeobjargproc)py_cRubython_RbArray_sq_ass_item, // sq_ass_item
  0, // sq_ass_slice
  0, // sq_contains
  0, // sq_inplace_concat
  0, // sq_inplace_repeat
};

Py_ssize_t
py_cRubython_RbArray_sq_length(PyObject *self) {
  DEBUG_MARKER;
  return RARRAY_LEN(RBARRAY_VALUE(self));
}

PyObject *
py_cRubython_RbArray_sq_item(PyObject *self, Py_ssize_t idx) {
  DEBUG_MARKER;
  PyObject *result = NULL;

  result = RB2PY(rb_funcall(RBARRAY_VALUE(self), rb_intern("[]"), 1, LONG2NUM(idx)));
  Py_RETURN(result);
}

int
py_cRubython_RbArray_sq_ass_item(PyObject *self, Py_ssize_t idx, PyObject *item) {
  DEBUG_MARKER;
  rb_funcall(RBARRAY_VALUE(self), rb_intern("[]="), 2, LONG2NUM(idx), PY2RB(item));
  return 0;
}

PyObject *
RbArray_WRAP(VALUE ptr) {
  DEBUG_MARKER;
  return RbObject_WRAP_BASE((PyTypeObject *)(py_cRbArray), ptr);
}

PyObject *
RbArray_Wrap(VALUE ptr) {
  DEBUG_MARKER;
  if (!RB_TYPE_P(ptr, RUBY_T_ARRAY)) {
    PyErr_Format(PyExc_TypeError, "Expected Ruby Array type, but got '%s'",
        rb_obj_classname(ptr));
    return NULL;
  }

  PyObject *wrapper = RbArray_WRAP(ptr);
  Py_RETURN(wrapper);
}

PyObject *
py_cRubython_RbArray_tp_richcompare(PyObject *self, PyObject *other, int op) {
  DEBUG_MARKER;
  VALUE rb_ary = ((py_Rubython_RbArray *)(self))->as.value;
  VALUE *ptr_a, *ptr_b;
  Py_ssize_t len_a, len_b;
  PyObject *result = Py_None;

  if (!((PyList_Check(self) || PyRbArray_Check(self)) &&
        (PyList_Check(other) || PyRbArray_Check(other)))) {
    result = Py_NotImplemented;
    goto out;
  }

  if (self == other) {
    switch (op) {
    case Py_EQ:case Py_LE:case Py_GE:
      result = Py_True;
      goto out;
    case Py_NE:case Py_LT:case Py_GT:
      result = Py_False;
      goto out;
    }
  }

  int other_native = 0;
  ptr_a = RARRAY_PTR(rb_ary);
  len_a = RARRAY_LEN(rb_ary);
  if (PyList_Check(other)) {
    PyListObject *other_ary = (PyListObject *)(other);
    // TODO: This should _not_ work.
    ptr_b = (VALUE *)(other_ary->ob_item);
    len_b = Py_SIZE(other_ary);
    other_native = 1;
  } else if (PyRbArray_Check(other)) {
    VALUE other_ary = ((py_Rubython_RbArray *)(other))->as.value;
    ptr_b = RARRAY_PTR(other_ary);
    len_b = RARRAY_LEN(other_ary);
  } else {
    // How the hell did we get here?
    DEBUG_MSG("WTF?");
    return NULL;
  }

  if ((len_a != len_b) && (op == Py_EQ || op == Py_NE)) {
    if (op == Py_EQ)
      result = Py_False;
    else
      result = Py_True;
    goto out;
  }

  int state;
  Py_ssize_t idx;
  for (idx = 0; idx < len_a && idx < len_b; ++idx) {
    VALUE other_item = other_native ? PY2RB(ptr_b[idx]) : ptr_b[idx];
    VALUE cmp = rb_funcall_protect(&state, ptr_a[idx], rb_intern("<=>"), 1, other_item);
    if (state != 0) {
      HandleRbErrors("Error attempting to compare %s and %s",
          StringValueCStr(ptr_a[idx]), StringValueCStr(other_item));
      return NULL;
    }
    if (NUM2INT(cmp) != 0)
      break;
  }

  if (idx >= len_a || idx >= len_b) {
    int cmp;
    switch (op) {
    case Py_LT: cmp = len_a <  len_b; break;
    case Py_LE: cmp = len_a <= len_b;
    case Py_EQ: cmp = len_a == len_b; break;
    case Py_NE: cmp = len_a != len_b; break;
    case Py_GT: cmp = len_a >  len_b; break;
    case Py_GE: cmp = len_a >= len_b; break;
    default: return NULL;
    }
    if (cmp)
      result = Py_True;
    else
      result = Py_False;
    goto out;
  }

  if (op == Py_EQ) {
    result = Py_False;
    goto out;
  }
  if (op == Py_NE) {
    result = Py_True;
    goto out;
  }

  // Compare the final item again using the proper operator
  VALUE other_item = other_native ? PY2RB(ptr_b[idx]) : ptr_b[idx];
  VALUE rb_cmp = rb_funcall_protect(&state, ptr_a[idx], rb_intern("<=>"), 1, other_item);
  if (state != 0) {
    HandleRbErrors("Error attempting to compare %s and %s",
        StringValueCStr(ptr_a[idx]), StringValueCStr(other_item));
    return NULL;
  }
  int icmp = NUM2INT(rb_cmp);
  int cmp;
  switch (op) {
  case Py_LT: cmp = icmp <  0; break;
  case Py_LE: cmp = icmp <= 0; break;
  case Py_EQ: cmp = icmp == 0; break;
  case Py_NE: cmp = icmp != 0; break;
  case Py_GT: cmp = icmp >  0; break;
  case Py_GE: cmp = icmp >= 0; break;
  default: return NULL;
  }
  if (cmp)
    result = Py_True;
  else
    result = Py_False;

out:
  Py_INCREF(result);
  return result;
}

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
