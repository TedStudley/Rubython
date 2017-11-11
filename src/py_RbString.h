#ifndef HEADER__PY_RB_STRING_H__INCLUDED
#define HEADER__PY_RB_STRING_H__INCLUDED

typedef union py_Rubython_RbStringStruct {
  py_Rubython_RbObject object;
  struct {
    PyObject_HEAD
    union {
      VALUE value;
      struct RString *rb_string;
    } as;
  } string;
} py_Rubython_RbString;

extern PyObject *py_cRbString;
extern PyTypeObject py_Rubython_RbString_type;

static PyBufferProcs py_cRubython_RbString_tp_as_buffer;

Py_ssize_t py_cRubython_RbString_bf_getreadbuffer(py_Rubython_RbString *self, Py_ssize_t segment, void **ptrptr);
Py_ssize_t py_cRubython_RbString_bf_getsegcount(py_Rubython_RbString *self, Py_ssize_t *lenp);
Py_ssize_t py_cRubython_RbString_bf_getcharbuffer(py_Rubython_RbString *self, Py_ssize_t segment, char **ptrptr);

PyObject *
py_cRubython_RbString_s_wrap(PyTypeObject *type, VALUE obj);
PyObject *RbStringWrap(VALUE obj);

static PyObject *
py_cRubython_RbString_str(py_Rubython_RbString *self);

void init_Rubython_RbString(void);

#endif // HEADER__PY_RB_STRING_H__INCLUDED
