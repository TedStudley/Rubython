#ifndef HEADER__PY_RB_HASH_H__INCLUDED
#define HEADER__PY_RB_HASH_H__INCLUDED

#include "py_RbObject.h"

typedef union py_Rubython_RbHashStruct {
  py_Rubython_RbObject object;
  struct {
    PyObject_HEAD
    union {
      VALUE value;
      struct RHash *rb_hash;
    } as;
  } hash;
} py_Rubython_RbHash;

extern PyObject *py_cRbHash;
extern PyTypeObject py_Rubython_RbHash_type;

PyObject *
py_cRubython_RbHash_s_wrap(PyTypeObject *type, VALUE obj);
PyObject *
RbHashWrap(VALUE obj);

void init_Rubython_RbHash(void);

#endif // HEADER__PY_RB_HASH_H__INCLUDED
