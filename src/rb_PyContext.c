#include "python/Python.h"
#include "python/frameobject.h"
#include "python/code.h"

#include "utility.h"
#include "conversion.h"
#include "rb_Rubython.h"
#include "rb_PyContext.h"
#include "py_Rubython.h"

#define GET_PYCONTEXT \
  rb_Rubython_PyContext *py_context; \
  TypedData_Get_Struct(self, rb_Rubython_PyContext, &rb_Rubython_PyContext_type, py_context);

#define CHECK_CONTEXT_HEALTH \
  if (pycontext_instance_p == NULL || \
      (*pycontext_instance_p) != self) \
    rb_raise(rb_eRuntimeError, "This PyContext is based on an outdated Python instance.");

VALUE rb_cPyContext;

static void rb_cRubython_PyContext__mark(void *ptr) {
  DEBUG_MARKER;
  rb_Rubython_PyContext *py_context = ptr;
}

static void rb_cRubython_PyContext__free(void *ptr) {
  DEBUG_MARKER;
  rb_Rubython_PyContext *py_context = ptr;
  // PyThreadState *py_thread = py_context->py_thread;
}

static size_t rb_cRubython_PyContext__memsize(const void *ptr) {
  DEBUG_MARKER;
  const rb_Rubython_PyContext *py_context = ptr;
  return sizeof(*py_context);
}

static const rb_data_type_t rb_Rubython_PyContext_type = {
  "Rubython/PyContext",
  {rb_cRubython_PyContext__mark, rb_cRubython_PyContext__free, rb_cRubython_PyContext__memsize,},
  #ifdef RUBY_TYPED_FREE_IMMEDIATELY
  0, 0,
  RUBY_TYPED_FREE_IMMEDIATELY,
  #endif
};

static PyThreadState *
InitPython(void) {
  DEBUG_MARKER;
  PyThreadState *thread_state;

  if (Py_IsInitialized()) {
    // How did we even get here, anyway?
    DEBUG_MSG("TODO: Handle Errors!");
    rb_raise(rb_eRuntimeError, "Rubython Python Context is already initialized!");
  }

  // Initialize Python
  Py_Initialize();
  PyEval_InitThreads();

  if (!PyEval_ThreadsInitialized()) {
    DEBUG_MSG("TODO: Handle Errors!");
    return NULL;
  }

  thread_state = PyThreadState_GET();
  if (!thread_state) {
    DEBUG_MSG("TODO: Handle Errors!");
    return NULL;
  }

  // Initialize Rubython on the other side
  initrubython_ext();

  // TODO: Handle Errors.
  return thread_state;
};

static VALUE *pycontext_instance_p = NULL;
static VALUE
rb_cRubython_PyContext_s_allocate(VALUE klass) {
  DEBUG_MARKER;
  PyThreadState *py_thread;
  int state = 0;
  VALUE obj;

  if (pycontext_instance_p == NULL) {
    DEBUG_MARKER;
    rb_Rubython_PyContext *instance = NULL;
    pycontext_instance_p = (VALUE *)malloc(sizeof(VALUE));
    (*pycontext_instance_p) = \
        TypedData_Make_Struct(rb_cPyContext, rb_Rubython_PyContext, &rb_Rubython_PyContext_type, instance);

    // Start up Python
    py_thread = InitPython();
    if (py_thread == NULL) {
      DEBUG_MSG("TODO: Handle Errors!");
      rb_raise(rb_eRuntimeError, "Failed to initialize Python");
    }

    // TODO: Ensure things aren't messed up if we hit an error
    instance->filename = 0;
    instance->py_frame = NULL;
  }
  return (*pycontext_instance_p);
}

static VALUE rb_cRubython_PyContext_s_get_instance(VALUE self) {
  DEBUG_MARKER;
  if (pycontext_instance_p == NULL)
    return Qnil;

  return (*pycontext_instance_p);
}

static VALUE rb_cRubython_PyContext_s_finalize(VALUE self) {
  DEBUG_MARKER;
  VALUE instance;
  rb_Rubython_PyContext *py_context;

  // TODO: Block teardown if the context is the host for the process
  // (How to figure that out?)

  if (pycontext_instance_p == NULL)
    return Qnil;

  // If there is a PyContext instance, tear it down.
  instance = (*pycontext_instance_p);
  free(pycontext_instance_p);
  pycontext_instance_p = NULL;

  // TODO: Clean up properly
  Py_Finalize();
  return Qnil;
}

static PyFrameObject *
initialize_python_context(rb_Rubython_PyContext *self) {
  DEBUG_MARKER;
  PyObject *py_main, *py_globals, *result;
  char *cfilename, *contents;
  PyFrameObject *py_frame;
  PyCodeObject *py_code;
  int state;

  if (!self->filename) {
    cfilename = "<rubython>";
    self->filename = rb_str_new_cstr(cfilename);
    contents = "\0";
  } else {
    cfilename = RSTRING_PTR(self->filename);
    contents = read_file(cfilename);
    if (contents == NULL) {
      DEBUG_MSG("TODO: Handle Errors!");
      rb_raise(rb_eRuntimeError, "Failed to read file '%s'", cfilename);
      return NULL;
    }
  }

  py_code = (PyCodeObject *)Py_CompileString(contents, cfilename, Py_file_input);
  if (py_code == NULL) {
    DEBUG_MSG("TODO: Handle Errors!");
    return NULL;
  }

  py_main = PyImport_AddModule("__main__");
  py_globals = PyModule_GetDict(py_main);
  if (py_main == NULL) {
    DEBUG_MSG("TODO: Handle Errors!");
    return NULL;
  }

  py_frame = PyFrame_New(PyThreadState_GET(), py_code, py_globals, py_globals);
  if (py_frame == NULL) {
    DEBUG_MSG("TODO: Handle Errors!");
    return NULL;
  }

  // Evaluate the initial context
  result = PyEval_EvalFrame(py_frame);
  PyThreadState_GET()->frame = py_frame;
  py_frame->f_back = NULL;

  return py_frame;
}

static VALUE rb_cRubython_PyContext_initialize(int argc, VALUE *argv, VALUE self) {
  DEBUG_MARKER;
  ASSERT_PY_INITIALIZED;
  GET_PYCONTEXT;
  PyFrameObject *py_frame;
  char *filename = NULL;

  if (py_context->filename)
    // Already initialized this instance
    return self;

  if (argc > 1) {
    // TODO: Actual error
    DEBUG_MSG("TODO: Handle Errors!");
    rb_raise(rb_eRuntimeError, "Expected just a single argument");
  } else if (argc == 1) {
    py_context->filename = argv[0];
  }

  py_frame = initialize_python_context(py_context);
  if (py_frame == NULL) {
    DEBUG_MSG("TODO: HANDLE ERRORS!");
    return -1;
  }

  py_context->py_frame = py_frame;
  return self;
}

static VALUE rb_cRubython_PyContext_builtins(VALUE self) {
  DEBUG_MARKER;
  GET_PYCONTEXT;
  CHECK_CONTEXT_HEALTH;
  return PY2RB(PyEval_GetBuiltins());
}

static VALUE rb_cRubython_PyContext_global_variables(VALUE self) {
  GET_PYCONTEXT;
  CHECK_CONTEXT_HEALTH;
  DEBUG_MARKER;
  return PY2RB(PyEval_GetGlobals());
}

static VALUE rb_cRubython_PyContext_local_variables(VALUE self) {
  DEBUG_MARKER;
  GET_PYCONTEXT;
  CHECK_CONTEXT_HEALTH;
  return PY2RB(PyEval_GetLocals());
}

static VALUE rb_cRubython_PyContext_modules(VALUE self) {
  DEBUG_MARKER;
  GET_PYCONTEXT;
  CHECK_CONTEXT_HEALTH;
  return Qnil;
}


static PyObject *
py_compile_and_run(VALUE self, const char *eval_str, int start) {
  DEBUG_MARKER;
  GET_PYCONTEXT;
  CHECK_CONTEXT_HEALTH;

  PyCodeObject *py_code = (PyCodeObject *)Py_CompileString(eval_str, "<rubython>", start);
  if (py_code == NULL) {
    DEBUG_MSG("TODO: Handle Errors!");
    PyErr_Print();
    return 0;
  }


  PyFrameObject *py_main = PyThreadState_GET()->frame;
  PyFrameObject *py_frame = PyFrame_New(PyThreadState_GET(), py_code,
                                        py_main->f_globals, py_main->f_locals);
  if (py_frame == NULL) {
    DEBUG_MSG("TODO: Handle Errors!");
    PyErr_Print();
    return 0;
  }
  Py_INCREF(py_frame);

  PyObject *py_result = PyEval_EvalFrame(py_frame);
  if (py_result == NULL) {
    DEBUG_MSG("TODO: Handle Errors!");
    PyErr_Print();
    return 0;
  }
  Py_DECREF(py_frame);

  return py_result;
}

static VALUE rb_cRubython_PyContext_py_eval(VALUE self, VALUE str) {
  DEBUG_MARKER;
  PyObject *py_str = NULL;
  Check_Type(str, T_STRING);
  py_str = PyString_FromString(RSTRING_PTR(str));
  return PY2RB(py_compile_and_run(self, RSTRING_PTR(str), Py_eval_input));
}

static VALUE rb_cRubython_PyContext_py_exec(VALUE self, VALUE str) {
  DEBUG_MARKER;
  PyObject *py_str = NULL;
  Check_Type(str, T_STRING);
  py_str = PyString_FromString(RSTRING_PTR(str));
  return PY2RB(py_compile_and_run(self, RSTRING_PTR(str), Py_single_input));
}

void Init_PyContext() {
  DEBUG_MARKER;
  rb_cPyContext = rb_define_class_under(rb_mRubython, "PyContext", rb_cObject);

  rb_define_alloc_func(rb_cPyContext, rb_cRubython_PyContext_s_allocate);
  rb_define_singleton_method(rb_cPyContext, "get_instance", rb_cRubython_PyContext_s_get_instance, 0);
  rb_define_singleton_method(rb_cPyContext, "finalize!", rb_cRubython_PyContext_s_finalize, 0);

  rb_define_method(rb_cPyContext, "initialize", rb_cRubython_PyContext_initialize, -1);

  rb_define_method(rb_cPyContext, "global_variables", rb_cRubython_PyContext_global_variables, 0);
  rb_define_method(rb_cPyContext, "local_variables", rb_cRubython_PyContext_local_variables, 0);

  rb_define_method(rb_cPyContext, "builtins", rb_cRubython_PyContext_builtins, 0);
  rb_define_method(rb_cPyContext, "modules", rb_cRubython_PyContext_modules, 0);

  rb_define_method(rb_cPyContext, "py_eval", rb_cRubython_PyContext_py_eval, 1);
  rb_define_method(rb_cPyContext, "py_exec", rb_cRubython_PyContext_py_exec, 1);
}
