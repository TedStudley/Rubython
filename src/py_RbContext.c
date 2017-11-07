#include "python/Python.h"
#include "ruby/ruby.h"
#include "python/structmember.h"

#include "ruby/vm.h"

#include "utility.h"
#include "conversion.h"
#include "py_Rubython.h"
#include "py_RbContext.h"
#include "rb_Rubython.h"

void (*Init_prelude)(void) = INIT_PRELUDE_ADDR;
void (*Init_enc)(void) = INIT_ENC_ADDR;
void (*Init_ext)(void) = INIT_EXT_ADDR;
typedef struct {
  int argc;
  char **argv;
} origarg_t;
origarg_t *origarg = ORIGARG_ADDR;

static PyMethodDef py_cRubython_RbContext_methods[];
static PyMemberDef py_cRubython_RbContext_members[];

PyObject *py_cRbContext;

static PyTypeObject py_Rubython_RbContext_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "rubython.RbContext",
    sizeof(py_Rubython_RbContext),
    0,
    (destructor)py_cRubython_RbContext_s_dealloc,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    Py_TPFLAGS_DEFAULT,
    "Rubython Ruby Execution Context",
    0,
    (inquiry)py_cRubython_RbContext_s_clear,
    0,
    0,
    0,
    0,
    py_cRubython_RbContext_methods, // RbContext_methods,
    py_cRubython_RbContext_members, // RbContext_members,
    0,
    0,
    0,
    0,
    0,
    0,
    (initproc)py_cRubython_RbContext_init,
    0,
    py_cRubython_RbContext_s_new,
};

static void
py_cRubython_RbContext_s_dealloc(py_Rubython_RbContext *self) {
  DEBUG_MARKER;
  PyObject_GC_UnTrack(self);
  py_cRubython_RbContext_s_finalize(self, NULL);
  py_cRubython_RbContext_s_clear(self);
  Py_TYPE(self)->tp_free((PyObject *)(self));
}

static int
py_cRubython_RbContext_s_clear(py_Rubython_RbContext *self) {
  DEBUG_MARKER;
  PyObject *tmp;

  tmp = self->filename;
  Py_CLEAR(self->filename);

  self->rb_binding = Qundef;
  rb_gc_unregister_address(&self->rb_binding);

  return 0;
}

static int InitRuby() {
  DEBUG_MARKER;
  int state = 0;

  RUBY_INIT_STACK;
  state = ruby_setup();
  if (state) {
    PyErr_SetString(PyExc_RuntimeError, "There was a problem attempting to set up Ruby!");
    return state;
  }

  ruby_init_loadpath();

  origarg->argc = 1;
  origarg->argv = malloc(sizeof(char *));
  origarg->argv[0] = "rubython";

  rb_const_remove(rb_cObject, rb_intern_const("ARGV"));
  ruby_prog_init();

  Init_enc();
  Init_ext();

  Init_prelude();
  rb_const_remove(rb_cObject, rb_intern_const("TMP_RUBY_PREFIX"));

  rb_define_module("Gem");

  // Initialize Rubython on the other side...
  Init_rubython_ext();

  return state;
}

static py_Rubython_RbContext *instance = NULL;
static PyObject *
py_cRubython_RbContext_s_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
  DEBUG_MARKER;
  int state = 0;
  if (instance == NULL) {
    instance = (py_Rubython_RbContext *)(type)->tp_alloc(type, 0);
    // Start up Ruby
    state = InitRuby();
    if (state) {
      PyErr_SetString(PyExc_RuntimeError, "There was a problem attempting to set up Ruby!");
      return NULL;
    }

    instance->rb_binding = rb_const_get(rb_cObject, rb_intern("TOPLEVEL_BINDING"));
    rb_gc_register_address(&instance->rb_binding);
  }
  Py_RETURN((PyObject *)(instance));
}

static PyObject *
py_cRubython_RbContext_s_finalize(py_Rubython_RbContext *cls, PyObject *dummy) {
  DEBUG_MARKER;
  py_Rubython_RbContext *tmp;
  int state = 0;

  if (instance == NULL)
    Py_RETURN_NONE;
  tmp = instance;
  instance = NULL;
  Py_XDECREF(instance);

  // Clean up after Ruby
  ruby_cleanup(state);
  Py_RETURN_NONE;
}

static VALUE eval_str(VALUE str) {
  return rb_funcall(rb_cObject, rb_intern("eval"), 2, str, instance->rb_binding);
}

static int
initialize_ruby_context(py_Rubython_RbContext *self) {
  DEBUG_MARKER;
  char *cfilename, *contents;
  VALUE result;
  int state;

  if (!self->filename) {
    cfilename = "<rubython>";
    self->filename = PyString_FromString(cfilename);
    contents = "\0";
  } else {
    cfilename = PyString_AS_STRING(self->filename);
    contents = read_file(cfilename);
    if (contents == NULL) {
      PyErr_Format(PyExc_IOError, "[Errno 2] No such file or directory: '%s'", cfilename);
      return -1;
    }
  }
  ruby_script(cfilename);
  ruby_init_loadpath();

  result = rb_protect(eval_str, rb_str_new_cstr(contents), &state);
  if (state) {
    result = rb_errinfo();
    rb_funcall(rb_mKernel, rb_intern("p"), 1, result);
  }

  return 0;
}

static int
py_cRubython_RbContext_init(py_Rubython_RbContext *self, PyObject *args, PyObject *kwds) {
  DEBUG_MARKER;
  PyObject *filename=NULL, *tmp;
  static char *kwlist[] = {"filename", NULL};

  if (self->filename)
    // We're already initialized. Just skip the rest
    return 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O",
                                   kwlist, &filename))
    return -1;

  if (filename) {
    tmp = self->filename;
    Py_INCREF(filename);
    self->filename = filename;
    Py_XDECREF(tmp);
  }

  if (initialize_ruby_context(self))
    return -1;

  return 0;
}

static PyMemberDef py_cRubython_RbContext_members[] = {
  {"filename", T_OBJECT_EX, offsetof(py_Rubython_RbContext, filename), 0,
    "loaded filename"},
  {NULL}
};

static PyObject *
py_cRubython_RbContext_global_variables(py_Rubython_RbContext *self, PyObject *dummy) {
  DEBUG_MARKER;
  VALUE retval = Qundef;
  int state = 0;

  retval = rb_funcall_protect(&state, rb_cObject, rb_intern("global_variables"), 0);
  if (state) {
    DEBUG_MSG("TODO: Handle errors");
    return NULL;
  }

  return RB2PY(retval);
}

static PyObject *
py_cRubython_RbContext_local_variables(py_Rubython_RbContext *self, PyObject *dummy) {
  DEBUG_MARKER;
  VALUE retval = Qundef;
  int state = 0;

  retval = rb_funcall_protect(&state, rb_cObject, rb_intern("global_variables"), 0);
  if (state) {
    DEBUG_MSG("TODO: Handle errors");
    return NULL;
  }

  return RB2PY(retval);
}

static PyObject *
py_cRubython_RbContext_constants(py_Rubython_RbContext *self, PyObject *dummy) {
  DEBUG_MARKER;
  VALUE retval = Qundef;
  int state = 0;

  retval = rb_funcall_protect(&state, rb_cModule, rb_intern("constants"), 0);
  if (state) {
    DEBUG_MSG("TODO: Handle errors");
    return NULL;
  }

  return RB2PY(retval);
}

static PyObject *
py_cRubython_RbContext_rb_eval(py_Rubython_RbContext *self, PyObject *args) {
  DEBUG_MARKER;
  int state;
  char *eval_cstr = NULL;
  VALUE retval = Qundef;
  if (!PyArg_ParseTuple(args, "s", &eval_cstr))
    return NULL;
  retval = rb_protect(eval_str, rb_str_new_cstr(eval_cstr), &state);
  if (state) {
    DEBUG_MSG("TODO: Handle error");
    return NULL;
  }
  return RB2PY(retval);
}

static PyMethodDef py_cRubython_RbContext_methods[] = {
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
