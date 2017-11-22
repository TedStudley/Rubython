#ifndef HEADER__PY_RB_HASH_H__INCLUDED
#define HEADER__PY_RB_HASH_H__INCLUDED

#include "Python.h"
#include "ruby.h"

#include "py_RbObject.h"

typedef struct py_Rubython_RbHashStruct {
  PyRbObject_HEAD;
  union {
    VALUE value;
    struct RHash *ptr;
  } as;
} py_Rubython_RbHash;
#define RBHASH_PTR(ptr) (((py_Rubython_RbHash *)(ptr))->as.ptr)
#define RBHASH_VALUE(ptr) (((py_Rubython_RbHash *)(ptr))->as.value)

extern PyObject *py_cRbHash;
extern PyTypeObject py_Rubython_RbHash_type;

PyObject *RbHash_WRAP(VALUE ptr);
PyObject *RbHash_Wrap(VALUE ptr);

void init_Rubython_RbHash(void);

#endif // HEADER__PY_RB_HASH_H__INCLUDED
