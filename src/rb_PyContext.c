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
  if (instance != py_context) \
    rb_raise(rb_eRuntimeError, "This PyContext is based on an outdated Python instance.");

VALUE rb_cPyContext;

static void rb_cRubython_PyContext__mark(void *ptr) {
  DEBUG_MARKER;
  rb_Rubython_PyContext *py_context = ptr;
}

static void rb_cRubython_PyContext__free(void *ptr) {
  DEBUG_MARKER;
  rb_Rubython_PyContext *py_context = ptr;
  PyThreadState *py_thread = py_context->py_thread;
  // Clear the base frame, if one was initialized
  // if (py_thread->frame != NULL) {
  //   Py_CLEAR(py_thread->frame);
  //   py_thread->frame = py_context->py_frame = NULL;
  // }
  // Py_EndInterpreter(py_context->py_thread);
  // ruby_xfree(py_context);
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

static int InitPython(void) {
  DEBUG_MARKER;
  if (Py_IsInitialized()) {
    // TODO: Better Errors
    rb_raise(rb_eRuntimeError, "Rubython Python Context is already initialized!");
  }

  Py_Initialize();
  PyEval_InitThreads();

  // TODO: Handle Errors.
  return 0;
};

static rb_Rubython_PyContext *instance = NULL;
static VALUE rb_cRubython_PyContext_s_allocate(VALUE klass) {
  DEBUG_MARKER;
  VALUE obj;

  if (instance == NULL) {
    InitPython();
    obj = TypedData_Make_Struct(klass, rb_Rubython_PyContext, &rb_Rubython_PyContext_type, instance);

    // Create a new python interpreter context
    PyThreadState *py_thread = Py_NewInterpreter();
    if (py_thread == NULL)
      rb_raise(rb_eRuntimeError, "Failed to construct a new PyInterpreter");
    // TODO: Ensure that we don't leave things in an inconsistent state in case
    // of error

    instance->py_thread = py_thread;
    instance->py_interp = py_thread->interp;
    instance->py_code = NULL;
    instance->py_locals = NULL;
    instance->py_globals = NULL;
  } else {
    obj = TypedData_Wrap_Struct(rb_cPyContext, &rb_Rubython_PyContext_type, instance);
  }
  return obj;
}

static VALUE rb_cRubython_PyContext_s_instance(VALUE self) {
  DEBUG_MARKER;
  if (instance == NULL)
    return Qnil;
  return TypedData_Wrap_Struct(self, &rb_Rubython_PyContext_type, instance);
}

static VALUE rb_cRubython_PyContext_s_finalize(VALUE self) {
  DEBUG_MARKER;
  // If there is a PyContext instance, tear it down.
  if (instance != NULL) {
    // Clear the base frame, if one was initialized
    if (instance->py_thread->frame != NULL) {
      Py_CLEAR(instance->py_thread->frame);
      instance->py_thread->frame = instance->py_frame = NULL;
    }
    // Py_EndInterpreter(instance->py_thread);
    Py_Finalize();
    ruby_xfree(instance);
    instance = NULL;
  }
  DEBUG_MARKER;
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

static VALUE rb_cRubython_PyContext_initialize(int argc, VALUE *argv, VALUE self) {
  DEBUG_MARKER;
  ASSERT_PY_INITIALIZED;
  GET_PYCONTEXT;

  if (py_context->py_code)
    // Already initialized this instance
    return self;

  // Create a new anonymous code object
  PyCodeObject *py_code;

  // If we were passed a file, try to run it...
  if (argc == 1) {
    const char *filename = RSTRING_PTR(argv[0]);
    py_code = Py_CompileFile(filename);
    if (py_code == NULL)
      PyErrorHandler("Unable to compile python file for interpretation: '%s'", filename);
  } else {
    py_code = PyCode_NewEmpty("<ruby>", "<rubython>", 0);
  }

  PyObject *py_locals = PyDict_New(),
          *py_globals = PyDict_New();
  Py_INCREF(py_locals); Py_INCREF(py_globals);
  if (PyDict_GetItemString(py_globals, "__builtins__") == NULL) {
    PyObject *bimod = py_context->py_interp->builtins;
    if (bimod == NULL)
      PyErrorHandler("Failed to retrieve builtins module");
    if (PyDict_SetItemString(py_globals, "__builtins__", bimod) < 0)
      PyErrorHandler("Failed to initialize __builtins__");
    Py_DECREF(bimod);
  };

  py_context->py_locals = py_locals;
  py_context->py_globals = py_globals;
  py_context->py_code = py_code;

  PyFrameObject *py_frame = PyFrame_New(py_context->py_thread, py_code, py_locals, py_globals);
  if (py_frame == NULL)
    PyErrorHandler("Unable to create new frame for code");
  Py_INCREF(py_frame);

  // Override the old frame builtins with the builtins from the interpreter
  PyObject *builtins = py_context->py_interp->builtins;
  Py_XDECREF(py_frame->f_builtins);
  py_frame->f_builtins = builtins;

  // Actually evaluate the frame
  PyEval_EvalFrame(py_frame);
  py_context->py_thread->frame = py_frame;

  return self;
}

static VALUE rb_cRubython_PyContext_builtins(VALUE self) {
  DEBUG_MARKER;
  GET_PYCONTEXT;
  CHECK_CONTEXT_HEALTH;
  PyObject *py_builtins = _PyThreadState_GetFrame(py_context->py_thread)->f_builtins;
  return PY2RB(py_builtins);
}

static VALUE rb_cRubython_PyContext_globals(VALUE self) {
  DEBUG_MARKER;
  GET_PYCONTEXT;
  CHECK_CONTEXT_HEALTH;
  PyObject *py_globals = _PyThreadState_GetFrame(py_context->py_thread)->f_globals;
  return PY2RB(py_globals);
}

static VALUE rb_cRubython_PyContext_locals(VALUE self) {
  DEBUG_MARKER;
  GET_PYCONTEXT;
  CHECK_CONTEXT_HEALTH;
  fprintf(stderr, "py_context is at %p\n", py_context);
  fprintf(stderr, "py_thread is at %p\n", py_context->py_thread);
  PyObject *py_locals = _PyThreadState_GetFrame(py_context->py_thread)->f_locals;
  return PY2RB(py_locals);
}

static VALUE rb_cRubython_PyContext_modules(VALUE self) {
  DEBUG_MARKER;
  GET_PYCONTEXT;
  CHECK_CONTEXT_HEALTH;
  PyObject *py_modules = py_context->py_interp->modules;
  return PY2RB(py_modules);
}


static VALUE compile_and_run(VALUE self, const char *eval_str, int start) {
  DEBUG_MARKER;
  GET_PYCONTEXT;
  CHECK_CONTEXT_HEALTH;

  PyCodeObject *py_code = (PyCodeObject *)Py_CompileString(eval_str, "<rubython>", start);
  if (py_code == NULL)
    PyErrorHandler("Could not compile source to eval");

  PyFrameObject *py_frame = PyFrame_New(py_context->py_thread, py_code,
                                        py_context->py_globals, py_context->py_locals);
  if (py_frame == NULL)
    PyErrorHandler("Failed to create new frame for compiled source");
  Py_XINCREF(py_frame);

  PyObject *py_result = PyEval_EvalFrame(py_frame);
  if (py_result == NULL)
    PyErrorHandler("Failed to evaluate compiled source");
  Py_XDECREF(py_frame);

  return PY2RB(py_result);
}

static VALUE rb_cRubython_PyContext_py_eval(VALUE self, VALUE rb_eval_str) {
  DEBUG_MARKER;
  Check_Type(rb_eval_str, T_STRING);
  return compile_and_run(self, RSTRING_PTR(rb_eval_str), Py_eval_input);
}

static VALUE rb_cRubython_PyContext_py_exec(VALUE self, VALUE rb_exec_str) {
  DEBUG_MARKER;
  Check_Type(rb_exec_str, T_STRING);
  return compile_and_run(self, RSTRING_PTR(rb_exec_str), Py_file_input);
}

void Init_PyContext() {
  DEBUG_MARKER;
  rb_cPyContext = rb_define_class_under(rb_mRubython, "PyContext", rb_cObject);

  rb_define_alloc_func(rb_cPyContext, rb_cRubython_PyContext_s_allocate);
  rb_define_singleton_method(rb_cPyContext, "instance", rb_cRubython_PyContext_s_instance, 0);
  rb_define_singleton_method(rb_cPyContext, "finalize!", rb_cRubython_PyContext_s_finalize, 0);

  rb_define_method(rb_cPyContext, "initialize", rb_cRubython_PyContext_initialize, -1);

  rb_define_method(rb_cPyContext, "global_variables", rb_cRubython_PyContext_globals, 0);
  rb_define_method(rb_cPyContext, "local_variables", rb_cRubython_PyContext_locals, 0);

  rb_define_method(rb_cPyContext, "builtins", rb_cRubython_PyContext_builtins, 0);
  rb_define_method(rb_cPyContext, "modules", rb_cRubython_PyContext_modules, 0);

  rb_define_method(rb_cPyContext, "py_eval", rb_cRubython_PyContext_py_eval, 1);
  rb_define_method(rb_cPyContext, "py_exec", rb_cRubython_PyContext_py_exec, 1);
}
