#ifndef HEADER__UTILITY_H__INCLUDED
#define HEADER__UTILITY_H__INCLUDED

#include "ruby/ruby.h"

#include "config.h"

#if defined(DEBUG)
# define DEBUG_MSG(c_str) \
    fprintf(stderr, "DEBUG: %s: %s:%d <%s>\n", c_str, __FILE__, __LINE__, __func__)
# define DEBUG_MARKER \
    DEBUG_MSG("<<MARK>>")
# define REPR(obj) \
    PyString_AS_STRING(PyObject_Repr((PyObject *)(obj)))
# define DEBUG_VAR(obj) \
    fprintf(stderr, "%s=%s\n", #obj, REPR(obj))
#else // defined(DEBUG)
# define DEBUG_MSG(c_str)
# define DEBUG_MARKER
# define REPR(obj)
# define DEBUG_VAR
#endif // defined(DEBUG)

#define Py_INCREF_RETURN(obj) \
  Py_INCREF(obj); return (obj)
#define Py_RETURN(obj) \
  Py_INCREF_RETURN(obj)
#define Py_XINCREF_RETURN(obj) \
  Py_XINCREF(obj); return (obj)

char *read_file(const char *filename);

VALUE rb_method_call_protect(int argc, const VALUE *argv, VALUE method, int *state);
VALUE rb_funcall_protect(int *state, VALUE obj, ID id, int argc, ...);
VALUE rb_obj_method_protect(VALUE obj, VALUE vid, int *state);

void PyErrorHandler(const char *format, ...);

#endif // HEADER__UTILITY_H__INCLUDED
