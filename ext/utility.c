#include "stdarg.h"
#include "stdio.h"

#include "ruby/ruby.h"
#include "python/Python.h"

#include "utility.h"

char save_error_type[1024], save_error_info[1024], save_c_error[1024];

char *
read_file(const char *filename) {
  char *contents;
  long fsize;
  FILE *fd;

  fd = fopen(filename, "rb");
  if (fd == NULL)
    // TODO: register an error somewhere
    return NULL;

  fseek(fd, 0, SEEK_END);
  fsize = ftell(fd);
  fseek(fd, 0, SEEK_SET);

  contents = malloc(fsize + 1);
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
  rb_method_call_args_t *args = (rb_method_call_args_t *)argsplat;
  return rb_method_call(args->argc, args->argv, args->method);
}

VALUE rb_method_call_protect(int argc, const VALUE *argv, VALUE method, int *state) {
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
  rb_funcall_args_t *args = (rb_funcall_args_t *)argsplat;
  return rb_funcallv(args->obj, args->id, args->argc, args->argv);
}

VALUE rb_funcall_protect(int *state, VALUE obj, ID id, int argc, ...) {
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

typedef struct rb_obj_method_args_struct {
  VALUE obj;
  VALUE vid;
} rb_obj_method_args_t;

static VALUE
__rb_obj_method_protect(VALUE argsplat) {
  rb_obj_method_args_t *args = (rb_obj_method_args_t *)argsplat;
  return rb_obj_method(args->obj, args->vid);
}

VALUE
rb_obj_method_protect(VALUE obj, VALUE vid, int *state) {
  rb_obj_method_args_t args = {obj, vid};
  return rb_protect(__rb_obj_method_protect, (VALUE) &args, state);
}

void PyErrorHandler(const char *format, ...) {
  PyObject *errobj, *errdata, *errtraceback, *pystring;
  PyErr_Fetch(&errobj, &errdata, &errtraceback);

  pystring = NULL;
  if (errobj != NULL &&
      (pystring = PyObject_Str(errobj)) != NULL &&
      (PyString_Check(pystring)))
    strcpy(save_error_type, PyString_AsString(pystring));
  else
    strcpy(save_error_type, "<unknown exception type>");
  // Py_XDECREF(pystring);

  pystring = NULL;
  if (errdata != NULL &&
      (pystring = PyObject_Str(errdata)) != NULL &&
      (PyString_Check(pystring)))
    strcpy(save_error_info, PyString_AsString(pystring));
  else
    strcpy(save_error_info, "<unknown exception data>");
  // Py_XDECREF(pystring);

  // Py_XDECREF(errobj);
  // Py_XDECREF(errdata);
  // Py_XDECREF(errtraceback);

  va_list args;
  va_start(args, format);
  vsprintf(save_c_error, format, args);
  vfprintf(stderr, format, args);
  va_end(args);
  rb_raise(rb_eRuntimeError, "%s :: Received python error %s: %s",
      save_c_error, save_error_type, save_error_info);
  // rb_raise(rb_eRuntimeError, "%s :: Received python error %s: %s",
  //       "nope", save_error_type, save_error_info);
}
