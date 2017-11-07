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

PyObject *
py_cRubython_RbString_s_wrap(PyTypeObject *type, VALUE obj);
PyObject *RbStringWrap(VALUE obj);

void init_Rubython_RbString(void);

#endif // HEADER__PY_RB_STRING_H__INCLUDED
