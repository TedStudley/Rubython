#include "Python.h"
#include "ruby.h"
#include "structmember.h"
#include "ruby/vm.h"
#include "malloc/malloc.h"

#include "ruby_shims.h"
#include "utility.h"
#include "conversion.h"
#include "py_Rubython.h"
#include "py_RbContext.h"
#include "rb_Rubython.h"
#include "py_error.h"

#define GET_INSTANCE \
  PyObject *instance = NULL; \
  if (rbcontext_instance_p != NULL) \
    instance = (*rbcontext_instance_p)

#define VERIFY_INSTANCE_HEALTH \
  if (rbcontext_instance_p == NULL || \
      (*rbcontext_instance_p) != self) { \
    PyErr_SetString(py_eRubythonError, "RbContext instance wrapper no longer has a valid context instance"); \
    return NULL; }

static PyMethodDef py_cRubython_RbContext_tp_methods[];
static PyMemberDef py_cRubython_RbContext_tp_members[];

int py_cRubython_RbObject_release(PyObject *self);

PyObject *py_cRbContext;

static PyTypeObject py_Rubython_RbContext_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "rubython.RbContext",
    sizeof(py_Rubython_RbContext),
    0,
    (destructor)py_cRubython_RbContext_tp_dealloc, // tp_dealloc
    0, // (printfunc) tp_print
    (getattrfunc)py_cRubython_RbContext_tp_getattr, // tp_getattr
    (setattrfunc)py_cRubython_RbContext_tp_setattr, // tp_setattr
    0, // (hasattrfunc) tp_hasattr
    0, // (reprfunc) tp_repr
    0, // (PyNumberMethods *) tp_as_number
    0, // (PySequenceMehtods *) tp_as_sequence
    0, // (PyMappingMethods *) tp_as_mapping
    0, // (hashfunc) tp_hash
    0, // (ternaryfunc) tp_call
    0, // (reprfunc) tp_str
    0, // (getattrofunc) tp_getattro
    0, // (setattrofunc) tp_setattro
    0, // (PyBufferProcs *) tp_as_buffer
    Py_TPFLAGS_DEFAULT |
      Py_TPFLAGS_HAVE_GC, // tp_flags
    "Rubython Ruby Execution Context", // tp_doc
    (traverseproc)py_cRubython_RbContext_tp_traverse, // tp_traverse
    (inquiry)py_cRubython_RbContext_tp_clear, // tp_clear
    0, // (richcmpfunc) tp_richcompare
    0, // (Py_ssize_t) tp_weaklistoffset
    0, // (getiterfunc) tp_iter
    0, // (iternextfunc) tp_iternext
    py_cRubython_RbContext_tp_methods, // tp_methods,
    py_cRubython_RbContext_tp_members, // tp_members,
    0, // (PyGetSetDef *) tp_getset
    0, // (_typeobject) *tp_base
    0, // (PyObject *) tp_dict
    0, // (descrgetfunc) tp_descr_get
    0, // (descrsetfunc) tp_descr_set
    0, // (Py_ssize_t) tp_dictoffset
    (initproc)py_cRubython_RbContext_tp_init, // tp_init
    0, // (allocfunc) tp_alloc
    py_cRubython_RbContext_tp_new, // tp_new
    py_cRubython_RbContext_tp_free, // (freefunc) tp_free
};

static void
py_cRubython_RbContext_tp_dealloc(PyObject *self) {
  DEBUG_MARKER;
  py_cRubython_RbContext_tp_clear(self);
  Py_TYPE(self)->tp_free((PyObject *)(self));
}

static void
py_cRubython_RbContext_tp_free(void *self) {
  DEBUG_MARKER;
  free(RBCONTEXT_OBJECTS(self).objects);
}

static int
py_cRubython_RbContext_tp_clear(PyObject *self) {
  DEBUG_MARKER;
  PyObject *tmp;

  tmp = RBCONTEXT_FILENAME(self);
  Py_CLEAR(RBCONTEXT_FILENAME(self));

  RBCONTEXT_BINDING(self) = Qundef;

  return 0;
}

static int InitRuby() {
  DEBUG_MARKER;
  int state = 0;

  RUBY_INIT_STACK;
  DEBUG_MARKER;
  DEBUG_MARKER;
  state = ruby_setup();
  DEBUG_MARKER;
  if (state) {
    DEBUG_MARKER;
    PyErr_SetString(PyExc_RuntimeError, "There was a problem attempting to set up Ruby!");
    return state;
  }

  origarg->argc = 1;
  origarg->argv = malloc(sizeof(char *));
  origarg->argv[0] = "rubython";
  DEBUG_MARKER;

  ruby_init_loadpath();
  DEBUG_MARKER;

  rb_const_remove(rb_cObject, rb_intern("ARGV"));
  DEBUG_MARKER;
  ruby_prog_init();
  DEBUG_MARKER;

  Init_enc();
  DEBUG_MARKER;
  Init_ext();
  DEBUG_MARKER;

  rb_define_module("Gem");
  DEBUG_MARKER;

  Init_prelude();
  DEBUG_MARKER;
  rb_const_remove(rb_cObject, rb_intern("TMP_RUBY_PREFIX"));
  DEBUG_MARKER;

  // Initialize Rubython on the other side...
  Init_rubython_ext();
  DEBUG_MARKER;

  return state;
}

PyObject **rbcontext_instance_p = NULL;
static PyObject *
py_cRubython_RbContext_tp_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
  DEBUG_MARKER;
  int state = 0;
  if (rbcontext_instance_p == NULL) {
    rbcontext_instance_p = (PyObject **)malloc(sizeof(PyObject *));
    (*rbcontext_instance_p) = (PyObject *)(type)->tp_alloc(type, 0);
    PyObject *instance = (*rbcontext_instance_p);

    RBCONTEXT_OBJECTS(instance).maxsize = 16;
    RBCONTEXT_OBJECTS(instance).objects = malloc(16*sizeof(PyObject *));
    for (size_t i = 0; i < 16; ++i)
      RBCONTEXT_OBJECTS(instance).objects[i] = NULL;
    RBCONTEXT_OBJECTS(instance).size = 0;

    // Start up Ruby
    state = InitRuby();
    if (state) {
      PyErr_SetString(PyExc_RuntimeError, "There was a problem attempting to set up Ruby!");
      return NULL;
    }

    RBCONTEXT_BINDING(instance) = Qundef;
    rb_gc_register_address(&RBCONTEXT_BINDING(instance));
    DEBUG_PRINTF("GC REGISTER %p\n", &RBCONTEXT_BINDING(instance));

    // Grab the master reference to the context instance
    Py_INCREF(instance);
  }
  return (*rbcontext_instance_p);
}

int
py_cRubython_RbContext_addObject(PyObject *self, PyObject *obj) {
  DEBUG_MARKER;
  py_Rubython_RbContext *context = (py_Rubython_RbContext *)(self);
  if (context->rb_objects.size >= context->rb_objects.maxsize) {
    size_t newsize = context->rb_objects.maxsize + 16;
    if (newsize < context->rb_objects.maxsize) {
      PyErr_SetString(py_eRubythonError, "Failed to allocate more space for object refs!");
      return -1;
    }
    context->rb_objects.objects = realloc(context->rb_objects.objects, newsize*sizeof(PyObject *));
    for (size_t i = context->rb_objects.maxsize; i < newsize; ++i)
      context->rb_objects.objects[i] = NULL;
    DEBUG_PRINTF("Allocated more space for objects (%zu -> %zu)\n", context->rb_objects.maxsize, newsize);
    context->rb_objects.maxsize = newsize;
  }

  if (context->rb_objects.packed) {
    context->rb_objects.objects[context->rb_objects.size++] = obj;
  } else {
    size_t idx;
    for (idx = 0; idx < context->rb_objects.size && context->rb_objects.objects[idx] != NULL; ++idx);
    context->rb_objects.objects[idx] = obj;
    if (idx >= context->rb_objects.size++)
      context->rb_objects.packed = 1;
  }
  return 0;
}

int
py_cRubython_RbContext_removeObject(PyObject *self, PyObject *obj) {
  DEBUG_MARKER;
  py_Rubython_RbContext *context = (py_Rubython_RbContext *)(self);
  size_t idx;
  for (idx = 0; idx < context->rb_objects.maxsize; ++idx)
    if (context->rb_objects.objects[idx] == obj)
      break;

  if (idx >= context->rb_objects.maxsize) {
    PyErr_Format(py_eRubythonError, "Failed to find object %p in object refs!", obj);
    return -1;
  }

  if (context->rb_objects.packed) {
    if (idx != context->rb_objects.size - 1)
      context->rb_objects.packed = 0;
  }
  context->rb_objects.objects[idx] = NULL;
  context->rb_objects.size--;
  return 0;
}

static PyObject *
py_cRubython_RbContext_s_get_instance(PyObject *cls, PyObject *dummy) {
  DEBUG_MARKER;
  PyObject *instance;

  if (rbcontext_instance_p == NULL)
    Py_RETURN_NONE;

  instance = (*rbcontext_instance_p);
  Py_RETURN(instance);
}

static PyObject *
py_cRubython_RbContext_s_finalize(PyObject *cls, PyObject *dummy) {
  DEBUG_MARKER;
  PyObject *instance;
  int state = 0;

  // TODO: Block teardown if the context is the host for the process
  // (How to figure that out?)

  if (rbcontext_instance_p == NULL ||
      (*rbcontext_instance_p == NULL))
    Py_RETURN_NONE;

  instance = (*rbcontext_instance_p);
  // Release all the held objects
  py_Rubython_RbContext *context = (py_Rubython_RbContext *)(instance);
  size_t idx = 0;
  if (context->rb_objects.packed) {
    while (context->rb_objects.size > 0)
      py_cRubython_RbObject_release(context->rb_objects.objects[--context->rb_objects.size]);
  } else {
    for (size_t idx = 0; idx < context->rb_objects.maxsize && context->rb_objects.size > 0; ++idx) {
      PyObject *obj = context->rb_objects.objects[idx];
      if (obj != NULL) {
        py_cRubython_RbObject_release(obj);
        context->rb_objects.size--;
      }
    }
  }
  // Release the master reference to the rbcontext instance
  Py_DECREF(instance);
  rbcontext_instance_p = NULL;

  // Allow the binding to be cleaned up
  // TODO: Wtf happens to any extant references?
  printf("GC UNREGISTER %p\n", &RBCONTEXT_BINDING(instance));
  rb_gc_unregister_address(&RBCONTEXT_BINDING(instance));

  // Clean up after Ruby
  rb_set_errinfo(Qnil);
  rb_garbage_collect();
  rb_objspace_t *tmp_objspace = (*ruby_current_vm_p)->objspace;
  printf("OBJSPACE AT %p\n", tmp_objspace);
  // stack_chunk_t *chunk = tmp_objspace->mark_stack.cache;
  // while (chunk != NULL) {
  //   for (idx = 0; idx < STACK_CHUNK_SIZE && chunk->data[idx] != 0; ++idx)
  //     if (malloc_size((void *)(chunk->data[idx]))) {
  //       obj_free(tmp_objspace, chunk->data[idx]);
  //       chunk->data[idx] = Qundef;
  //     }
  //   chunk = chunk->next;
  // }
  // chunk = tmp_objspace->mark_stack.chunk;
  // while (chunk != NULL) {
  //   for (idx = 0; idx < STACK_CHUNK_SIZE && chunk->data[idx] != 0; ++idx)
  //     if (malloc_size((void *)(chunk->data[idx]))) {
  //       obj_free(tmp_objspace, chunk->data[idx]);
  //       chunk->data[idx] = Qundef;
  //     }
  //   chunk = chunk->next;
  // }

  state = ruby_cleanup(0);
  if (state) {
    HandleRbErrors("TODO: Better Errors!");
    return NULL;
  }

  // This is dumb.
  (*ruby_setup_initialized_p) = 0;
  (*frozen_strings_p) = NULL;
  (*ruby_vm_global_method_state_p) = 1;
  (*ruby_vm_global_constant_state_p) = 1;
  (*ruby_vm_class_serial_p) = 1;
  (*default_proc_for_compat) = 0;
  // Really dumb
  // TODO: This probably leaks memory.
  rb_cBasicObject = 0;
  rb_cObject = 0;
  rb_cModule = 0;
  rb_cClass = 0;
  rb_cString = 0;
  // TODO: DON'T HARDCODE THIS VALUE
  global_symbols_p->last_id = 209;

  Py_RETURN_NONE;
}

static VALUE eval_str(VALUE str) {
  DEBUG_MARKER;
  GET_INSTANCE;
  return rb_funcall(rb_cObject, rb_intern("eval"), 2, str, RBCONTEXT_BINDING(instance));
}

static int
initialize_ruby_context(PyObject *self) {
  DEBUG_MARKER;
  char *cfilename, *contents;
  VALUE result;
  int state;

  if (!RBCONTEXT_FILENAME(self)) {
    cfilename = "<rubython>";
    RBCONTEXT_FILENAME(self) = PyString_FromString(cfilename);
    contents = "\0";
  } else {
    cfilename = PyString_AS_STRING(RBCONTEXT_FILENAME(self));
    contents = read_file(cfilename);
    if (contents == NULL) {
      PyErr_Format(PyExc_IOError, "[Errno 2] No such file or directory: '%s'", cfilename);
      return -1;
    }
  }
  ruby_script(cfilename);
  ruby_init_loadpath();

  // Grab the current binding
  RBCONTEXT_BINDING(self) = rb_funcall((*ruby_current_vm_p)->top_self, rb_intern("binding"), 0);

  // Evaluate the initial context
  result = rb_protect(eval_str, rb_str_new_cstr(contents), &state);
  if (state) {
    HandleRbErrors("TODO: Better Errors!");
    return -1;
  }

  return 0;
}

static int py_cRubython_RbContext_tp_traverse(PyObject *self, visitproc visit, void *arg) {
  DEBUG_MARKER;
  return 0;
}

static int
py_cRubython_RbContext_tp_init(PyObject *self, PyObject *args, PyObject *kwds) {
  DEBUG_MARKER;
  PyObject *filename=NULL, *tmp;
  static char *kwlist[] = {"filename", NULL};

  if (RBCONTEXT_FILENAME(self) != NULL)
    // We're already initialized. Just skip the rest
    return 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O",
                                   kwlist, &filename))
    return -1;

  if (filename) {
    RBCONTEXT_FILENAME(self) = filename;
  }

  if (initialize_ruby_context(self)) {
    HandleRbErrors("TODO: Better Errors!");
    return -1;
  }
  Py_INCREF(RBCONTEXT_FILENAME(self));

  return 0;
}

static PyObject *
py_cRubython_RbContext_tp_getattr(PyObject *self, const char *attr_name) {
  DEBUG_MARKER;
  PyObject *attr = NULL;
  VALUE rb_attr = Qundef;
  int state = 0;

  // Check if there's an attr on our RbContext, first
  attr = PyObject_GenericGetAttr(self, PyUnicode_FromString(attr_name));
  if (attr == NULL) {
    // Catch the error and clear it
    PyErr_Clear();

    // Try to look up a ruby attribute
    rb_attr = rb_eval_string_protect(attr_name, &state);
    if (state != 0) {
      HandleRbErrors("Error looking up attr");
      return NULL;
    }

    DEBUG_MARKER;
    attr = RB2PY(rb_attr);
  }
  DEBUG_MARKER;
  return attr;
}

static PyObject *
py_cRubython_RbContext_tp_setattr(PyObject *self, const char *attr_name, PyObject *value) {
  // TODO: Support context.foo = bar
  return NULL;
}

static PyMemberDef py_cRubython_RbContext_tp_members[] = {
  {"filename", T_OBJECT_EX, offsetof(py_Rubython_RbContext, filename), 0,
    "loaded filename"},
  {NULL}
};

static PyObject *
py_cRubython_RbContext_global_variables(PyObject *self, PyObject *dummy) {
  DEBUG_MARKER;
  VERIFY_INSTANCE_HEALTH;
  VALUE retval = Qundef;
  int state = 0;

  retval = rb_funcall_protect(&state, RBCONTEXT_BINDING(self), rb_intern("global_variables"), 0);
  if (state) {
    HandleRbErrors("TODO: Better Errors!");
    return NULL;
  }

  return RB2PY(retval);
}

static PyObject *
py_cRubython_RbContext_local_variables(PyObject *self, PyObject *dummy) {
  DEBUG_MARKER;
  VERIFY_INSTANCE_HEALTH;
  VALUE retval = Qundef;
  int state = 0;

  retval = rb_funcall_protect(&state, RBCONTEXT_BINDING(self), rb_intern("local_variables"), 0);
  if (state) {
    HandleRbErrors("TODO: Better Errors!");
    return NULL;
  }

  return RB2PY(retval);
}

static PyObject *
py_cRubython_RbContext_constants(PyObject *self, PyObject *dummy) {
  DEBUG_MARKER;
  VERIFY_INSTANCE_HEALTH;
  VALUE retval = Qundef;
  int state = 0;

  retval = rb_funcall_protect(&state, rb_cModule, rb_intern("constants"), 0);
  if (state) {
    HandleRbErrors("TODO: Better Errors!");
    return NULL;
  }

  return RB2PY(retval);
}

static PyObject *
py_cRubython_RbContext_rb_eval(PyObject *self, PyObject *args) {
  DEBUG_MARKER;
  VERIFY_INSTANCE_HEALTH;
  VALUE rb_result = Qundef;
  char *eval_cstr = NULL;
  PyObject *result;
  int state;

  if (!PyArg_ParseTuple(args, "s", &eval_cstr))
    return NULL;

  rb_result = rb_protect(eval_str, rb_str_new_cstr(eval_cstr), &state);
  if (state) {
    HandleRbErrors("TODO: Better Errors!");
    return NULL;
  }

  return RB2PY(rb_result);
}

PyObject *
py_cRubython_RbContext_is_valid(PyObject *self) {
  DEBUG_MARKER;
  if ((rbcontext_instance_p != NULL) &&
      ((*rbcontext_instance_p) == self))
    Py_RETURN_TRUE;
  Py_RETURN_FALSE;
}

static PyMethodDef py_cRubython_RbContext_tp_methods[] = {
  {"get_instance", (PyCFunction)py_cRubython_RbContext_s_get_instance, METH_NOARGS | METH_CLASS,
    "Returns the currently-active RbContext instance if one exists, and None otherwise."},
  {"finalize", (PyCFunction)py_cRubython_RbContext_s_finalize, METH_NOARGS | METH_CLASS,
    "Finalizes the currently-active RbContext instance, invalidating all extant RbContext wrappers."},

  {"global_variables", (PyCFunction)py_cRubython_RbContext_global_variables, METH_NOARGS,
    "Returns a list of the names of all global variables in the current ruby context"},
  {"local_variables", (PyCFunction)py_cRubython_RbContext_local_variables, METH_NOARGS,
    "Returns a list of the names of all local variables in the current ruby context"},

  {"constants", (PyCFunction)py_cRubython_RbContext_constants, METH_NOARGS,
    "Returns a list of the names of all constants in the current ruby context"},

  {"rb_eval", (PyCFunction)py_cRubython_RbContext_rb_eval, METH_VARARGS,
    "Evaluate a string within the ruby context"},

  {"is_valid", (PyCFunction)py_cRubython_RbContext_is_valid, METH_NOARGS,
    "Returns whether or not the RbContext is still valid"},

  {NULL} // Sentinel
};


void
init_Rubython_RbContext(void) {
  DEBUG_MARKER;
  if (PyType_Ready(&py_Rubython_RbContext_type) < 0) {
    DEBUG_MSG("TODO: Handle Errors!");
    return;
  }

  py_cRbContext = (PyObject *)&py_Rubython_RbContext_type;
  Py_INCREF(py_cRbContext);
  PyModule_AddObject(py_mRubython, "RbContext", py_cRbContext);
}
