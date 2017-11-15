#include "Python.h"
#include "ruby.h"
#include "structmember.h"

#include "utility.h"
#include "conversion.h"
#include "py_RbTypes.h"
#include "py_RbObject.h"

static PyMethodDef py_cRubython_RbObject_methods[];
static PyMemberDef py_cRubython_RbObject_members[];

PyObject *py_cRbObject;

PyTypeObject py_Rubython_RbObject_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "rubython.rubython_ext.rb_types.RbObject",
    sizeof(py_Rubython_RbObject),
    0,
    (destructor)py_cRubython_RbObject_s_dealloc, // tp_dealloc
    0, // (printfunc) tp_print
    (getattrfunc)py_cRubython_RbObject_getattr, // tp_getattr
    (setattrfunc)py_cRubython_RbObject_setattr, // tp_setattr
    0, // (PyAsyncMethods *) tp_as_async
    (reprfunc)py_cRubython_RbObject_repr, // tp_repr
    0, // (PyNumberMethods *) tp_as_number
    0, // (PySequenceMethods *) tp_as_sequence
    0, // (PyMappingMethods *) tp_as_mapping
    0, // (hashfunc) tp_hash
    (ternaryfunc)py_cRubython_RbObject_call, // tp_call
    (reprfunc)py_cRubython_RbObject_str, // tp_str
    0, // (getattrofunc) tp_getattro
    0, // (setattrofunc) tp_setattro
    0, // (PyBufferProcs *) tp_as_buffer
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE, // tp_flags
    "Rubython Ruby Object", // tp_doc
    0, // (traverseproc) tp_traverse
    (inquiry)py_cRubython_RbObject_s_clear, // tp_clear
    0, // (richcmpfunc) tp_richcompare
    0, // (Py_ssize_t) tp_weaklistoffset
    0, // (getiterfunc) tp_iter
    0, // (iternextfunc) tp_iternext
    py_cRubython_RbObject_methods, // tp_methods
    py_cRubython_RbObject_members, // tp_members
    0, // (PyGetSetDef *) tp_getset
    0, // (_typeobject) *tp_base
    0, // (PyObject *) tp_dict;
    0, // (descrgetfunc) tp_descr_get
    0, // (descrsetfunc) tp_descr_set
    0, // (Py_ssize_t) tp_dictoffset;
    (initproc)py_cRubython_RbObject_init, // tp_init
    0, // (allocfunc) tp_alloc
    (newfunc)py_cRubython_RbObject_s_new, // tp_new
    0, // (freefunc) tp_free
};

static void
py_cRubython_RbObject_s_dealloc(py_Rubython_RbObject *self) {
  DEBUG_MARKER;
  PyObject_GC_UnTrack(self);
  py_cRubython_RbObject_s_clear(self);
  Py_TYPE(self)->tp_free((PyObject *)(self));
}

static PyObject *
py_cRubython_RbObject_getattr(py_Rubython_RbObject *self, const char *attr_name) {
  DEBUG_MARKER;
  PyObject *attr = NULL;
  VALUE rb_attr = Qundef;
  int state = 0;

  // Check if there's an attr on our PyObject, first
  attr = PyObject_GenericGetAttr((PyObject *)(self), PyUnicode_FromString(attr_name));
  if (attr == NULL) {
    // Catch the error and clear it
    PyErr_Clear();

    // Try to look up a ruby attribute
    rb_attr = rb_attr_get(self->as.value, rb_intern(attr_name));
    if (rb_attr == Qnil) {
      // Next, check if there's an attr(method/ivar) by that name on self
      rb_attr = rb_obj_method_protect(self->as.value, rb_str_new_cstr(attr_name), &state);
      if (state) {
        // TODO: Handle errors
        DEBUG_MSG("TODO: Handle errors");
        rb_attr = rb_errinfo();
        rb_funcall(rb_mKernel, rb_intern("p"), 1, rb_attr);
        Py_RETURN_NONE;
      }
    }
    attr = RB2PY(rb_attr);
  }
  Py_RETURN(attr);
}

static PyObject *
py_cRubython_RbObject_setattr(py_Rubython_RbObject *self, const char *attr_name, PyObject *value) {
  DEBUG_MARKER;
  PyErr_SetString(PyExc_RuntimeError, "Use rb_eval to set attributes, not setattr.");
  // TODO: Support setting ruby instance vars
  return NULL;
}

static PyObject *
py_cRubython_RbObject_repr(py_Rubython_RbObject *self) {
  DEBUG_MARKER;
  VALUE rb_repr = Qundef;
  int state = 0;

  rb_repr = rb_funcall_protect(&state, self->as.value, rb_intern("inspect"), 0);
  if (state) {
    // TODO: Handle errors
    DEBUG_MSG("TODO: Handle errors");
  }

  char *obj_repr = StringValueCStr(rb_repr);
  PyObject *py_repr = PyString_FromString(obj_repr);
  Py_INCREF(py_repr);
  return py_repr;
}

static PyObject *
py_cRubython_RbObject_call(py_Rubython_RbObject *self, PyObject *args, PyObject *other) {
  DEBUG_MARKER;
  VALUE rb_result = Qundef, *rb_argv_ = NULL;
  PyObject *result = NULL;
  int argc, state = 0;
  ID id_call;

  id_call = rb_intern("call");
  if (!rb_respond_to(self->as.value, id_call)) {
    DEBUG_MSG("TODO: Better Errors!");
    // TODO: Better errors
    PyErr_SetString(PyExc_RuntimeError, "Object is not callable");
    return NULL;
  }

  argc = convertPyArgs(args, &rb_argv_);
  rb_result = rb_funcall2_protect(self->as.value, id_call, argc, rb_argv_, &state);
  free(rb_argv_);
  if (state) {
    DEBUG_MSG("TODO: Handle Errors!");
    rb_result = rb_errinfo();
    rb_funcall(rb_mKernel, rb_intern("p"), 1, rb_result);
  }
  result = RB2PY(rb_result);
  Py_RETURN(result);
}

static PyObject *
py_cRubython_RbObject_str(py_Rubython_RbObject *self) {
  DEBUG_MARKER;
  VALUE rb_str = Qundef;
  int state = 0;

  rb_str = rb_funcall_protect(&state, self->as.value, rb_intern("to_s"), 0);
  if (state) {
    // TODO: Handle errors
    DEBUG_MSG("TODO: Handle Errors");
  }

  char *obj_str = StringValueCStr(rb_str);
  PyObject *py_str = PyString_FromString(obj_str);
  Py_INCREF(py_str);
  return py_str;
}

static int
py_cRubython_RbObject_s_clear(py_Rubython_RbObject *self) {
  DEBUG_MARKER;
  // This might be a problem, since it isn't actually an RObject pointer...
  self->as.value = Qundef;
  rb_gc_unregister_address(&self->as.value);

  return 0;
}

PyObject *
py_cRubython_RbObject_s_wrap(PyTypeObject *type, VALUE rb_object) {
  DEBUG_MARKER;
  py_Rubython_RbObject *self = (py_Rubython_RbObject *)(type->tp_alloc(type, 0));

  ((py_Rubython_RbObject *)(self))->as.value = rb_object;
  rb_gc_register_address(&self->as.value);

  return (PyObject *)(self);
}
PyObject *
RbObjectWrap(VALUE obj) {
  DEBUG_MARKER;
  return py_cRubython_RbObject_s_wrap(&py_Rubython_RbObject_type, obj);
}

static PyObject *
py_cRubython_RbObject_s_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
  DEBUG_MARKER;
  PyErr_SetString(PyExc_RuntimeError, "Cannot allocate an object of type RbObject from python!");
  return NULL;
}

static int
py_cRubython_RbObject_init(py_Rubython_RbObject *self, PyObject *args, PyObject *kwds) {
  DEBUG_MARKER;
  PyErr_SetString(PyExc_RuntimeError, "Cannot init an object of type RbObject from python!");
  return -1;
}

static PyMemberDef py_cRubython_RbObject_members[] = {
  {NULL}
};

static PyMethodDef py_cRubython_RbObject_methods[] = {
  {NULL}
};

void init_Rubython_RbObject(void) {
  DEBUG_MARKER;
  if (PyType_Ready(&py_Rubython_RbObject_type) < 0) {
    // TODO: Actual errors
    DEBUG_MSG("TODO: Handle Errors");
    return;
  }

  py_cRbObject = (PyObject *)&py_Rubython_RbObject_type;
  Py_INCREF(py_cRbObject);
  PyModule_AddObject(py_mRbTypes, "RbObject", py_cRbObject);
}
