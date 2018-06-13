#include "Python.h"
#include "ruby.h"
#include "structmember.h"

#include "utility.h"
#include "conversion.h"
#include "py_RbContext.h"
#include "py_RbTypes.h"
#include "py_RbObject.h"
#include "py_error.h"

static PyMethodDef py_cRubython_RbObject_tp_methods[];
static PyMemberDef py_cRubython_RbObject_tp_members[];

PyObject *py_cRbObject;

PyTypeObject py_Rubython_RbObject_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "rubython.rubython_ext.rb_types.RbObject",
    sizeof(py_Rubython_RbObject),
    0,
    (destructor)py_cRubython_RbObject_tp_dealloc, // tp_dealloc
    0, // (printfunc) tp_print
    (getattrfunc)py_cRubython_RbObject_tp_getattr, // tp_getattr
    0, // (setattrfunc)py_cRubython_RbObject_tp_setattr, // tp_setattr
    0, // (PyAsyncMethods *) tp_as_async
    (reprfunc)py_cRubython_RbObject_tp_repr, // tp_repr
    0, // (PyNumberMethods *) tp_as_number
    0, // (PySequenceMethods *) tp_as_sequence
    0, // (PyMappingMethods *) tp_as_mapping
    0, // (hashfunc) tp_hash
    (ternaryfunc)py_cRubython_RbObject_tp_call, // tp_call
    (reprfunc)py_cRubython_RbObject_tp_str, // tp_str
    0, // (getattrofunc) tp_getattro
    0, // (setattrofunc) tp_setattro
    0, // (PyBufferProcs *) tp_as_buffer
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE |
        Py_TPFLAGS_HAVE_GC, // tp_flags
    "Rubython Ruby Object", // tp_doc
    (traverseproc)py_cRubython_RbObject_tp_traverse, // (traverseproc) tp_traverse
    (inquiry)py_cRubython_RbObject_tp_clear, // tp_clear
    0, // (richcmpfunc) tp_richcompare
    0, // (Py_ssize_t) tp_weaklistoffset
    0, // (getiterfunc) tp_iter
    0, // (iternextfunc) tp_iternext
    py_cRubython_RbObject_tp_methods, // tp_methods
    py_cRubython_RbObject_tp_members, // tp_members
    0, // (PyGetSetDef *) tp_getset
    0, // (_typeobject) *tp_base
    0, // (PyObject *) tp_dict;
    0, // (descrgetfunc) tp_descr_get
    0, // (descrsetfunc) tp_descr_set
    0, // (Py_ssize_t) tp_dictoffset;
    (initproc)py_cRubython_RbObject_tp_init, // tp_init
    0, // (allocfunc) tp_alloc
    (newfunc)py_cRubython_RbObject_tp_new, // tp_new
    0, // (freefunc) tp_free
};
#define PyRbObject_Check(ob) PyObject_TypeCheck((ob), (PyTypeObject *)(py_cRbObject))

static void
py_cRubython_RbObject_tp_dealloc(PyObject *self) {
  DEBUG_MARKER;
  if (py_cRubython_RbContext_removeObject(RBOBJECT_CONTEXT(self), self)) {
    printf("This should never happen!\n");
    DEBUG_MARKER;
  };
  py_cRubython_RbObject_tp_clear(self);
  Py_TYPE(self)->tp_free(self);
}

static int
py_cRubython_RbObject_tp_clear(PyObject *self) {
  DEBUG_MARKER;

  if (RBOBJECT_CONTEXT(self)) {
    py_cRubython_RbObject_release(self);
    rb_gc_unregister_address(&RBOBJECT_VALUE(self));
  }

  RBOBJECT_VALUE(self) = Qundef;
  return 0;
}

int
py_cRubython_RbObject_release(PyObject *self) {
  DEBUG_MARKER;
  printf("%p\n", self);
  py_cRubython_RbContext_removeObject(RBOBJECT_CONTEXT(self), self);
  DEBUG_MARKER;
  rb_gc_unregister_address(&RBOBJECT_VALUE(self));
  DEBUG_MARKER;
  Py_CLEAR(RBOBJECT_CONTEXT(self));
  DEBUG_MARKER;
  return 0;
}

static int
py_cRubython_RbObject_tp_traverse(PyObject *self, visitproc visit, void *arg) {
  DEBUG_MARKER;
  return 0;
}

static PyObject *
py_cRubython_RbObject_tp_getattr(PyObject *self, const char *attr_name) {
  DEBUG_MARKER;
  PyObject *attr = NULL;
  VALUE rb_attr = Qundef;
  int state = 0;

  // Check if there's an attr on our PyObject, first
  attr = PyObject_GenericGetAttr(self, PyUnicode_FromString(attr_name));
  if (attr == NULL) {
    // Catch the error and clear it
    PyErr_Clear();

    // Try to look up a ruby attribute
    rb_attr = rb_attr_get(RBOBJECT_VALUE(self), rb_intern(attr_name));
    if (rb_attr == Qnil) {
      // Next, check if there's an attr(method/ivar) by that name on self
      rb_attr = rb_obj_method_protect(RBOBJECT_VALUE(self), rb_str_new_cstr(attr_name), &state);
      if (state) {
        HandleRbErrors("TODO: Better Errors");
        return NULL;
      }
    }
    attr = RB2PY(rb_attr);
  }
  Py_RETURN(attr);
}

static PyObject *
py_cRubython_RbObject_tp_repr(PyObject *self) {
  DEBUG_MARKER;
  PyObject *py_repr = NULL;
  VALUE rb_repr = Qundef;
  int state = 0;

  rb_repr = rb_funcall_protect(&state, RBOBJECT_VALUE(self), rb_intern("inspect"), 0);
  if (state) {
    HandleRbErrors("TODO: Better Errors");
    return NULL;
  }

  py_repr = Rb2PyRaw_String(rb_repr);
  printf("String at %p\n", py_repr);
  return py_repr;
}

static PyObject *
py_cRubython_RbObject_tp_call(PyObject *self, PyObject *args, PyObject *other) {
  DEBUG_MARKER;
  VALUE rb_result = Qundef, *rb_argv_ = NULL;
  PyObject *result = NULL;
  int argc, state = 0;
  ID id_call;

  id_call = rb_intern("call");
  if (!rb_respond_to(RBOBJECT_VALUE(self), id_call)) {
    HandleRbErrors("Object is not callable");
    return NULL;
  }

  argc = convertPyArgs(args, &rb_argv_);
  rb_result = rb_funcall2_protect(RBOBJECT_VALUE(self), id_call, argc, rb_argv_, &state);
  free(rb_argv_);
  if (state) {
    HandleRbErrors("TODO: Better Errors!");
    return NULL;
  }
  result = RB2PY(rb_result);
  return result;
}

static PyObject *
py_cRubython_RbObject_tp_str(PyObject *self) {
  DEBUG_MARKER;
  VALUE rb_str = Qundef;
  int state = 0;

  rb_str = rb_funcall_protect(&state, RBOBJECT_VALUE(self), rb_intern("to_s"), 0);
  if (state) {
    HandleRbErrors("TODO: Better Errors");
    return NULL;
  }

  PyObject *py_str = Rb2PyRaw_String(rb_str);
  Py_RETURN(py_str);
}

PyObject *
RbObject_WRAP_BASE(PyTypeObject *type, VALUE ptr) {
  DEBUG_MARKER;
  PyObject *self = type->tp_alloc(type, 0);
  ((py_Rubython_RbObject *)(self))->as.value = ptr;
  if (type->tp_init(self, NULL, NULL))
    return NULL;
  rb_gc_register_address(&RBOBJECT_VALUE(self));
  return self;
}

PyObject *
RbObject_WRAP(VALUE ptr) {
  DEBUG_MARKER;

  return RbObject_WRAP_BASE((PyTypeObject *)(py_cRbObject), ptr);
}

static PyObject *
py_cRubython_RbObject_tp_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
  DEBUG_MARKER;
  PyErr_SetString(py_eRubythonError, "Cannot allocate an object of type RbObject from python!");
  return NULL;
}

static int
py_cRubython_RbObject_tp_init(PyObject *self, PyObject *args, PyObject *kwds) {
  DEBUG_MARKER;

  if (rbcontext_instance_p == NULL)
    // Something funky is going on
    return -1;

  PyObject *context = (*rbcontext_instance_p);
  RBOBJECT_CONTEXT(self) = context;
  Py_INCREF(context);
  if (py_cRubython_RbContext_addObject(context, self)) {
    printf("This should never happen!\n");
    DEBUG_MARKER;
  }
  rb_gc_register_address(&RBOBJECT_VALUE(self));
  return 0;
}

static PyObject *
py_cRubython_RbObject_is_valid(PyObject *self, PyObject *args, PyObject *kwds) {
  DEBUG_MARKER;
  return py_cRubython_RbContext_is_valid(RBOBJECT_CONTEXT(self));
}

static PyMemberDef
py_cRubython_RbObject_tp_members[] = {
  {"__context__", T_OBJECT, offsetof(py_Rubython_RbObject, context), 0,
    PyDoc_STR("The owning context")},
  {NULL}
};

static PyMethodDef
py_cRubython_RbObject_tp_methods[] = {
  {"is_valid", (PyCFunction)py_cRubython_RbObject_is_valid, METH_NOARGS,
    "Returns whether or not the wrapper reference is still valid"},
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
