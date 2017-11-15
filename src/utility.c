#include "stdarg.h"
#include "stdio.h"

#include "ruby/ruby.h"
#include "python/Python.h"

#include "utility.h"

char save_error_type[1024], save_error_info[1024], save_c_error[1024];

char *
read_file(const char *filename) {
  DEBUG_MARKER;
  char *contents;
  long fsize;
  FILE *fd;

  fd = fopen(filename, "rb");
  if (fd == NULL) {
    DEBUG_MSG("TODO: Handle Errors!");
    return NULL;
  }

  fseek(fd, 0, SEEK_END);
  fsize = ftell(fd);
  fseek(fd, 0, SEEK_SET);
  if (fsize <= 0) {
    DEBUG_MSG("TODO: Handle Errors!");
    return NULL;
  }

  contents = malloc(fsize + 1);
  if (contents == NULL) {
    DEBUG_MSG("TODO: Handle Errors!");
    return NULL;
  }
  fread(contents, fsize, 1, fd);
  fclose(fd);

  contents[fsize] = '\0';
  return contents;
}

typedef struct rb_method_call_args_struct {
  int argc;
  const VALUE *argv;
  VALUE method;
} rb_method_call_args_t;

static VALUE
__rb_method_call_protect(VALUE argsplat) {
  DEBUG_MARKER;
  rb_method_call_args_t *args = (rb_method_call_args_t *)argsplat;
  return rb_method_call(args->argc, args->argv, args->method);
}

VALUE rb_method_call_protect(int argc, const VALUE *argv, VALUE method, int *state) {
  DEBUG_MARKER;
  rb_method_call_args_t args = {argc, argv, method};
  return rb_protect(__rb_method_call_protect, (VALUE) &args, state);
}

typedef struct rb_funcall_args_struct {
  VALUE obj;
  ID id;
  int argc;
  VALUE *argv;
} rb_funcall_args_t;

static VALUE
__rb_funcall_protect(VALUE argsplat) {
  DEBUG_MARKER;
  rb_funcall_args_t *args = (rb_funcall_args_t *)argsplat;
  return rb_funcallv(args->obj, args->id, args->argc, args->argv);
}

VALUE rb_funcall_protect(int *state, VALUE obj, ID id, int argc, ...) {
  DEBUG_MARKER;
  VALUE *rbargv = NULL, result;
  va_list argv;

  va_start(argv, argc);
  rbargv = (VALUE *)malloc(sizeof(VALUE)*argc);
  // TODO: Handle NULL rbargv
  for (int i = 0; i < argc; ++i)
    rbargv[i] = va_arg(argv, VALUE);
  va_end(argv);

  rb_funcall_args_t args = {obj, id, argc, rbargv};
  result = rb_protect(__rb_funcall_protect, (VALUE) &args, state);
  free(rbargv);
  return result;
}

VALUE rb_funcall2_protect(VALUE obj, ID id, int argc, const VALUE *argv, int *state) {
  DEBUG_MARKER;
  VALUE result;
  rb_funcall_args_t args = {obj, id, argc, argv};
  result = rb_protect(__rb_funcall_protect, (VALUE) &args, state);
  return result;
}

typedef struct rb_obj_method_args_struct {
  VALUE obj;
  VALUE vid;
} rb_obj_method_args_t;

static VALUE
__rb_obj_method_protect(VALUE argsplat) {
  DEBUG_MARKER;
  rb_obj_method_args_t *args = (rb_obj_method_args_t *)argsplat;
  return rb_obj_method(args->obj, args->vid);
}

VALUE
rb_obj_method_protect(VALUE obj, VALUE vid, int *state) {
  DEBUG_MARKER;
  rb_obj_method_args_t args = {obj, vid};
  return rb_protect(__rb_obj_method_protect, (VALUE) &args, state);
}

void HandlePyErrors(const char *format, ...) {
  DEBUG_MARKER;
  PyObject *err_type = NULL, *err_value = NULL, *err_traceback = NULL,
           *py_str = NULL;
  if (!PyErr_Occurred())
    return

  PyErr_Fetch(&err_type, &err_value, &err_traceback);

  if (err_type != NULL &&
      (py_str = PyObject_Str(err_type)) != NULL &&
      (PyString_Check(py_str)))
    strcpy(save_error_type, PyString_AsString(py_str));
  else
    strcpy(save_error_type, "<unknown exception type>");
  Py_XDECREF(py_str);

  if (err_value != NULL &&
      (py_str = PyObject_Str(err_value)) != NULL &&
      (PyString_Check(py_str)))
    strcpy(save_error_info, PyString_AsString(py_str));
  else
    strcpy(save_error_info, "<unkown exception data>");
  Py_XDECREF(py_str);

  // TODO: Traceback!
  Py_XDECREF(err_type);
  Py_XDECREF(err_value);
  Py_XDECREF(err_traceback);

  va_list args;
  va_start(args, format);
  vsprintf(save_c_error, format, args);
  vfprintf(stderr, format, args);
  va_end(args);

  // TODO: Dynamic error type based on Python error (or PyException Exception/Wrapper?)
  rb_raise(rb_eRuntimeError, "%s :: recieved python error %s: %s",
      save_c_error, save_error_type, save_error_info);
}
