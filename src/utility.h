#ifndef HEADER__UTILITY_H__INCLUDED
#define HEADER__UTILITY_H__INCLUDED

#include "ruby.h"

#include "config.h"

#if defined(DEBUG)
# define DEBUG_PRINTF(...) \
    fprintf(stderr, "DEBUG: "); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, ": %s:%d <%s>\n", __FILE__, __LINE__, __func__);
# define DEBUG_MSG(c_str) \
    fprintf(stderr, "DEBUG: %s: %s:%d <%s>\n", c_str, __FILE__, __LINE__, __func__)
# define DEBUG_MARKER \
    DEBUG_MSG("<<MARK>>")
# define REPR(obj) \
    PyString_AS_STRING(PyObject_Repr((PyObject *)(obj)))
# define DEBUG_VAR(obj) \
    fprintf(stderr, "%s=%s\n", #obj, REPR(obj))
# define PYSELF_DEBUG\
    DEBUG_PRINTF("(SELF) WRAPPER AT %p", self);\
    DEBUG_PRINTF("  TYPE=%s", self->ob_type->tp_name);\
    DEBUG_PRINTF("  REFCOUNT=%lu", self->ob_refcnt);
#else // defined(DEBUG)
# define DEBUG_PRINTF(c_str, ...)
# define DEBUG_MSG(c_str)
# define DEBUG_MARKER
# define REPR(obj)
# define DEBUG_VAR
# define PYSELF_DEBUG
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
VALUE rb_funcall2_protect(VALUE obj, ID id, int argc, const VALUE *argv, int *state);
VALUE rb_obj_method_protect(VALUE obj, VALUE vid, int *state);

void HandlePyErrors(const char *format, ...);
void HandleRbErrors(const char *format, ...);

#endif // HEADER__UTILITY_H__INCLUDED
