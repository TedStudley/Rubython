#include "python/Python.h"
#include "python/frameobject.h"
#include "python/code.h"

#include "utility.h"
#include "conversion.h"
#include "rb_Rubython.h"
#include "rb_PyContext.h"

#define GET_PYCONTEXT \
  rb_Rubython_PyContext *py_context; \
  TypedData_Get_Struct(self, rb_Rubython_PyContext, &rb_Rubython_PyContext_type, py_context);

#define CHECK_CONTEXT_HEALTH \
  if ((*pycontext_instance_p) != self) \
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

  if (pycontext_instance_p == NULL)
    return Qnil;

  // If there is a PyContext instance, tear it down.
  instance = (*pycontext_instance_p);
  free(pycontext_instance_p);
  pycontext_instance_p = NULL;

  // TODO: Clean up properly
  // TypedData_Get_Struct((*pycontext_instance_p), rb_Rubython_PyContext, &rb_Rubython_PyContext_type, py_context);
  // Py_CLEAR(py_context->py_thread->frame);

  // Clear the base frame, if one was initialized
  // if (instance->py_thread->frame != NULL) {
  //   Py_CLEAR(instance->py_thread->frame);
  //   instance->py_thread->frame = instance->py_frame = NULL;
  // }
  // Py_EndInterpreter(instance->py_thread);
  Py_Finalize();
  // ruby_xfree(instance);
  // instance = NULL;
  return Qnil;
}

static PyCodeObject *Py_CompileFile(const char *filename) {
  DEBUG_MARKER;
  // Open the file for reading
  FILE *fd = fopen(filename, "rb");
  if (fd == NULL)
    return NULL;

  // Find the size of the file
  size_t fsize;
  fseek(fd, 0, SEEK_END);
  fsize = ftell(fd);
  fseek(fd, 0, SEEK_SET);

  // Read the contents of the file
  char *contents = malloc(fsize + 1);
  fread(contents, fsize, 1, fd);
  fclose(fd);

  // Null-terminate the string, just in case.
  contents[fsize] = '\0';
  PyObject *py_code = Py_CompileString(contents, filename, Py_file_input);
  free(contents);

  return (PyCodeObject *)(py_code);
}

static PyObject *
py_eval_str(PyObject *eval_str) {
  DEBUG_MARKER;
  PyCompilerFlags cf;
  PyObject *tmp, *result;
  PyObject *globals, *locals;
  char *str;

  cf.cf_flags = 0;
  if (PyUnicode_Check(eval_str)) {
    tmp = PyUnicode_AsUTF8String(eval_str);
    if (tmp == NULL)
      return NULL;
    eval_str = tmp;
    cf.cf_flags |= PyCF_SOURCE_IS_UTF8;
  }
  if (PyString_AsStringAndSize(eval_str, &str, NULL)) {
    Py_XDECREF(tmp);
    return NULL;
  }
  while (*str == ' ' || *str == '\t')
    str++;

  globals = PyEval_GetGlobals();
  locals = PyEval_GetLocals();
  PyEval_MergeCompilerFlags(&cf);
  result = PyRun_StringFlags(str, Py_eval_input, globals, locals, &cf);
  Py_XDECREF(tmp);

  return result;
};

static PyObject *
py_exec_str(PyObject *exec_str) {
  DEBUG_MARKER;
  PyCompilerFlags cf;
  PyObject *tmp;
  PyObject *globals, *locals;
  char *str;

  cf.cf_flags = 0;
  if (PyUnicode_Check(exec_str)) {
    tmp = PyUnicode_AsUTF8String(exec_str);
    if (tmp == NULL)
      return NULL;
    exec_str = tmp;
    cf.cf_flags |= PyCF_SOURCE_IS_UTF8;
  }
  if (PyString_AsStringAndSize(exec_str, &str, NULL)) {
    Py_XDECREF(tmp);
    return NULL;
  }
  while (*str == ' ' || *str == '\t')
    str++;

  globals = PyEval_GetGlobals();
  locals = PyEval_GetLocals();
  PyEval_MergeCompilerFlags(&cf);
  PyRun_StringFlags(str, Py_file_input, globals, locals, &cf);
  Py_XDECREF(tmp);

  return Py_None;
};

static int
initialize_python_context(rb_Rubython_PyContext *self) {
  DEBUG_MARKER;
  char *cfilename, *contents;
  PyObject *result;
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
      return -1;
    }
  }

  self->py_frame = PyEval_GetFrame();

  py_exec_str(PyString_FromString(contents));

  return 0;
}

static VALUE rb_cRubython_PyContext_initialize(int argc, VALUE *argv, VALUE self) {
  DEBUG_MARKER;
  ASSERT_PY_INITIALIZED;
  GET_PYCONTEXT;
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

  if (initialize_python_context(py_context))
    return -1;

  return self;

  // Create a new anonymous code object
  // PyCodeObject *py_code;

  // If we were passed a file, try to run it...
  // if (argc == 1) {
  //   const char *filename = RSTRING_PTR(argv[0]);
  //   py_code = Py_CompileFile(filename);
  //   if (py_code == NULL)
  //     PyErrorHandler("Unable to compile python file for interpretation: '%s'", filename);
  // } else {
  //   py_code = PyCode_NewEmpty("<ruby>", "<rubython>", 0);
  // }

  // PyObject *py_locals = PyDict_New(),
  //         *py_globals = PyDict_New();
  // Py_INCREF(py_locals); Py_INCREF(py_globals);
  // if (PyDict_GetItemString(py_globals, "__builtins__") == NULL) {
  //   PyObject *bimod = py_context->py_interp->builtins;
  //   if (bimod == NULL)
  //     PyErrorHandler("Failed to retrieve builtins module");
  //   if (PyDict_SetItemString(py_globals, "__builtins__", bimod) < 0)
  //     PyErrorHandler("Failed to initialize __builtins__");
  //   Py_DECREF(bimod);
  // };

  // py_context->py_locals = py_locals;
  // py_context->py_globals = py_globals;
  // py_context->py_code = py_code;

  // PyFrameObject *py_frame = PyFrame_New(py_context->py_thread, py_code, py_locals, py_globals);
  // if (py_frame == NULL)
  //   PyErrorHandler("Unable to create new frame for code");
  // Py_INCREF(py_frame);

  // // Override the old frame builtins with the builtins from the interpreter
  // PyObject *builtins = py_context->py_interp->builtins;
  // Py_XDECREF(py_frame->f_builtins);
  // py_frame->f_builtins = builtins;

  // // Actually evaluate the frame
  // PyEval_EvalFrame(py_frame);
  // py_context->py_thread->frame = py_frame;

  // return self;
}

static VALUE rb_cRubython_PyContext_builtins(VALUE self) {
  DEBUG_MARKER;
  GET_PYCONTEXT;
  CHECK_CONTEXT_HEALTH;
  return PY2RB(PyEval_GetBuiltins());
}

static VALUE rb_cRubython_PyContext_global_variables(VALUE self) {
  DEBUG_MARKER;
  GET_PYCONTEXT;
  CHECK_CONTEXT_HEALTH;
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


// static VALUE compile_and_run(VALUE self, const char *eval_str, int start) {
//   DEBUG_MARKER;
//   GET_PYCONTEXT;
//   CHECK_CONTEXT_HEALTH;
// 
//   PyCodeObject *py_code = (PyCodeObject *)Py_CompileString(eval_str, "<rubython>", start);
//   if (py_code == NULL) {
//     DEBUG_MSG("TODO: Handle Errors!");
//     PyErr_Print();
//     return 0;
//   }
// 
//   PyFrameObject *py_frame = PyFrame_New(py_context->py_thread, py_code,
//                                         py_context->py_globals, py_context->py_locals);
//   if (py_frame == NULL) {
//     DEBUG_MSG("TODO: Handle Errors!");
//     PyErr_Print();
//     return 0;
//   }
//   Py_XINCREF(py_frame);
// 
//   PyObject *py_result = PyEval_EvalFrame(py_frame);
//   if (py_result == NULL) {
//     DEBUG_MSG("TODO: Handle Errors!");
//     PyErr_Print();
//     return 0;
//   }
//   Py_XDECREF(py_frame);
// 
//   return PY2RB(py_result);
// }

static VALUE rb_cRubython_PyContext_py_eval(VALUE self, VALUE str) {
  DEBUG_MARKER;
  PyObject *py_str = NULL;
  Check_Type(str, T_STRING);
  py_str = PyString_FromString(RSTRING_PTR(str));
  return PY2RB(py_eval_str(py_str));
  // compile_and_run(self, RSTRING_PTR(rb_eval_str), Py_eval_input);
}

static VALUE rb_cRubython_PyContext_py_exec(VALUE self, VALUE str) {
  DEBUG_MARKER;
  PyObject *py_str = NULL;
  Check_Type(str, T_STRING);
  py_str = PyString_FromString(RSTRING_PTR(str));
  return PY2RB(py_exec_str(py_str));
  // return compile_and_run(self, RSTRING_PTR(rb_exec_str), Py_single_input);
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
