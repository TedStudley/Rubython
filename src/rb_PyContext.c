#include "Python.h"
#include "frameobject.h"
#include "code.h"

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

VALUE *pycontext_instance_p = NULL;
static VALUE
rb_cRubython_PyContext_s_allocate(VALUE klass) {
  DEBUG_MARKER;
  PyThreadState *py_thread;
  int state = 0;
  VALUE obj;

  if (pycontext_instance_p == NULL) {
    rb_Rubython_PyContext *instance = NULL;
    pycontext_instance_p = (VALUE *)malloc(sizeof(VALUE));
    (*pycontext_instance_p) = \
        TypedData_Make_Struct(rb_cPyContext, rb_Rubython_PyContext, &rb_Rubython_PyContext_type, instance);

    // Check if there's a current python context already. If there is, we're
    // most likely running as the guest interpreter.
    if (PyThreadState_GET() == NULL) {
      DEBUG_MARKER;
      // Start up Python
      py_thread = InitPython();
      if (py_thread == NULL) {
        DEBUG_MSG("TODO: Handle Errors!");
        rb_raise(rb_eRuntimeError, "Failed to initialize Python");
      }
    } else {
      DEBUG_PRINTF("RUNNING AS GUEST INTERPRETER!");
    }
  }
  return (*pycontext_instance_p);
}

/* Returns the current PyContext instance. If there is no active Python context,
 * either because one was never created or the previous one was torn down, then
 * nil will be returned.
 *
 * @return [PyContext] the current PyContext instance
 */
static VALUE rb_cRubython_PyContext_s_get_instance(VALUE self) {
  DEBUG_MARKER;
  if (pycontext_instance_p == NULL)
    return Qnil;

  return (*pycontext_instance_p);
}

/* Completely tears down the current PyContext instance.
 *
 * This will finalize the current Python context so that a new Python context
 * can be created, and will completely invalidate all borrowed references from
 * the currently active Python context.
 *
 * @return [nil]
 */
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

/* Document-method PyContext.new
 *
 * Test
 */

/*
 * @returns [PyContext]
 */
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

static VALUE
rb_cRubython_PyContext_is_validq(VALUE self) {
  DEBUG_MARKER;
  return (((pycontext_instance_p == NULL) || ((*pycontext_instance_p) != self)) ? Qfalse : Qtrue);
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

static PyObject *
pycontext_load_name(PyFrameObject *f, const char *n) {
  DEBUG_MARKER;
  PyObject *v;

  v = PyObject_GetItem(f->f_locals, PyString_FromString(n));
  if (v == NULL && PyErr_Occurred()) {
    if (!PyErr_ExceptionMatches(PyExc_KeyError))
      HandlePyErrors("Error attempting lookup from frame locals");
    PyErr_Clear();
  }

  if (v == NULL) {
    v = PyDict_GetItemString(f->f_globals, n);
    if (v == NULL) {
      v = PyDict_GetItemString(f->f_builtins, n);
      if (v == NULL) {
        DEBUG_MSG("TODO: Better Errors");
        return NULL;
      }
    }
  }

  return v;
}

/*
 * @return [Object] the matching object from the Python context
 */
static VALUE
rb_cRubython_PyContext_method_missing(int argc, VALUE *argv, VALUE self) {
  DEBUG_MARKER;
  GET_PYCONTEXT;
  CHECK_CONTEXT_HEALTH;
  VALUE mid, org_mid, sym, v;
  PyObject *py_obj;
  const char *mname;

  rb_check_arity(argc, 1, UNLIMITED_ARGUMENTS);
  mid = org_mid = argv[0];
  sym = rb_check_symbol(&mid);
  if (!NIL_P(sym)) mid = rb_sym2str(sym);
  mname = StringValueCStr(mid);
  if (!mname)
    rb_raise(rb_eRuntimeError, "fail: unknown method or property");

  py_obj = pycontext_load_name(PyThreadState_GET()->frame, mname);
  if (py_obj == NULL) {
    HandlePyErrors("Error attempting to load Python name");
    return rb_call_super(argc, argv);
  }

  if (PyCallable_Check(py_obj) && !PyType_Check(py_obj)) {
    VALUE rb_args = rb_ary_new_from_values(argc - 1, argv + 1);
    PyObject *py_args;
    convertRbArgs(rb_args, &py_args);
    // TODO: Support keyword args
    py_obj = PyObject_Call(py_obj, py_args, NULL);
    if (py_obj == NULL) {
      HandlePyErrors("TODO: Handle Errors!");
      return Qundef;
    }
  }

  return PY2RB(py_obj);
}

static PyObject *
py_compile_and_run(VALUE self, const char *eval_str, int start) {
  DEBUG_MARKER;
  GET_PYCONTEXT;
  CHECK_CONTEXT_HEALTH;

  PyCodeObject *py_code = (PyCodeObject *)Py_CompileString(eval_str, "<rubython>", start);
  if (py_code == NULL)
    return NULL;

  PyFrameObject *py_main = PyThreadState_GET()->frame;
  PyFrameObject *py_frame = PyFrame_New(PyThreadState_GET(), py_code,
                                        py_main->f_globals, py_main->f_locals);
  if (py_frame == NULL)
    return NULL;
  Py_INCREF(py_frame);

  PyObject *py_result = PyEval_EvalFrame(py_frame);
  if (py_result == NULL) {
    Py_DECREF(py_frame);
    return NULL;
  }
  Py_DECREF(py_frame);

  return py_result;
}

/* Evaluates a string within the Python context.
 *
 * @example Returning a string from Python
 *   py_context.rb_eval("'foo'") # => "foo" (PyString)
 * @example Returning an integer from Python
 *   py_context.rb_eval("2") # => 2 (Fixnum)
 * @example Returning the result of an expression from Python
 *   py_context.rb_eval("7 * 6") # => 42 (Fixnum)
 * @example Returning the result of a list comprehension from Python
 *   py_context.rb_eval("[x * 2 for x in [1,2,3]]") # => [2, 4, 6] (PyList)
 *
 * @param [String] eval_str the string to be evaluated within the Python
 *     context
 * @return [Object] the result of evaluation from Python
 */
static VALUE rb_cRubython_PyContext_py_eval(VALUE self, VALUE str) {
  DEBUG_MARKER;
  PyObject *py_result = NULL;
  Check_Type(str, T_STRING);

  py_result = py_compile_and_run(self, RSTRING_PTR(str), Py_eval_input);
  if (py_result == NULL) {
    HandlePyErrors("Error evaluating python code");
    return Qundef;
  }

  return PY2RB(py_result);
}

/* Executes a string within the Python context.
 *
 * @example Assigns the value of a Python variable
 *   py_context.py_exec("foo = 'bar'")
 * @example Executes the Python print statement
 *   py_context.py_exec("print('foobar')") # foobar
 *
 * @param [String] eval_str the string to be executed within the Python context
 * @return [nil]
 */
static VALUE rb_cRubython_PyContext_py_exec(VALUE self, VALUE str) {
  DEBUG_MARKER;
  PyObject *py_result = NULL;
  Check_Type(str, T_STRING);

  py_result = py_compile_and_run(self, RSTRING_PTR(str), Py_single_input);
  if (PyErr_Occurred()) {
    HandlePyErrors("Error evaluating python code");
    return Qundef;
  }
  return PY2RB(py_result);
}

/*
 * Document-class: Rubython::PyContext
 *
 * A singleton wrapper class for the current Python context.
 *
 * This class serves as the Ruby side of the bridge between the Python
 * and Ruby contexts.
 */
void Init_PyContext(void) {
  DEBUG_MARKER;
  rb_cPyContext = rb_define_class_under(rb_mRubython, "PyContext", rb_cObject);

  rb_define_alloc_func(rb_cPyContext, rb_cRubython_PyContext_s_allocate);
  rb_define_singleton_method(rb_cPyContext, "get_instance", rb_cRubython_PyContext_s_get_instance, 0);
  rb_define_singleton_method(rb_cPyContext, "finalize!", rb_cRubython_PyContext_s_finalize, 0);

  rb_define_method(rb_cPyContext, "initialize", rb_cRubython_PyContext_initialize, -1);

  rb_define_method(rb_cPyContext, "is_valid?", rb_cRubython_PyContext_is_validq, 0);

  rb_define_method(rb_cPyContext, "method_missing", rb_cRubython_PyContext_method_missing, -1);

  rb_define_method(rb_cPyContext, "global_variables", rb_cRubython_PyContext_global_variables, 0);
  rb_define_method(rb_cPyContext, "local_variables", rb_cRubython_PyContext_local_variables, 0);

  rb_define_method(rb_cPyContext, "py_eval", rb_cRubython_PyContext_py_eval, 1);
  rb_define_method(rb_cPyContext, "py_exec", rb_cRubython_PyContext_py_exec, 1);
}
