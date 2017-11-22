#ifndef HEADER__RB_PY_STRING_H__INCLUDED
#define HEADER__RB_PY_STRING_H__INCLUDED

#include "ruby.h"
#include "Python.h"
#include "stringobject.h"

typedef PyStringObject rb_Rubython_PyString;

extern VALUE rb_cPyString;

static void rb_cRubython_PyString__mark(void *ptr);
static void rb_cRubython_PyString__free(void *ptr);
static size_t rb_cRubython_PyString__memsize(const void *ptr);

extern const rb_data_type_t rb_Rubython_PyString_type;

VALUE PyString_WRAP(PyObject *ptr);
static inline VALUE PyStringWrap(PyObject *ptr) {
  DEBUG_MARKER;

  if (!PyString_Check(ptr))
    rb_raise(rb_eTypeError, "Expected python str type, but got %s",
        Py_TYPE(ptr)->tp_name);

  return PyString_WRAP(ptr);
}

#define PyString_PTR(rval)\
  PyObject_PTR_OF(rval, &rb_Rubython_PyString_type)
#define PYSTRING_PTR(obj) \
  (PyStringObject *)rb_check_typeddata((obj), &rb_Rubython_PyString_type)
#define PYSTRING_OBJPTR(obj) \
  (PyObject *)rb_check_typeddata((obj), &rb_Rubython_PyString_type)

static st_index_t rb_cRubython_PyString_hash(PyStringObject *py_string);

VALUE rb_cRubython_PyString_hash_m(VALUE self);

VALUE rb_cRubython_PySTring_eq(VALUE self, VALUE other);

VALUE rb_cRubython_PyString_to_s(VALUE self);
VALUE rb_cRubython_PyString_inspect(VALUE self);

void Init_PyString(void);

#endif // HEADER__RB_PY_STRING_H__INCLUDED
