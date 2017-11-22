#ifndef HEADER__CONVERSION_H__INCLUDED
#define HEADER__CONVERSION_H__INCLUDED

#include "ruby.h"
#include "Python.h"

#define RB_ASSERT_PYTYPE(type) \
  if (!Py##type##_Check(obj)) \
    rb_raise(rb_eTypeError, "Expected Python ##type## type, but got '%s'", \
        Py_Type(obj)->tp_name)

#define PY_ASSERT_RBTYPE(type) \
  if (!RB_TYPE_P(obj, (RUBY_T_##type))) { \
    PyErr_Format(PyExc_TypeError, "Expected Ruby " #type " type, but got '%s'", \
        rb_obj_classname(obj)); \
    return NULL; }

// Master converter. Try to convert to other supported types, fall back to Object.
VALUE Py2Rb_Object(PyObject *obj);

#define PY2RB(obj) \
  Py2Rb_Object((PyObject *)(obj))

// String Convertor
VALUE Py2Rb_String(PyObject *obj);
VALUE Py2RbRaw_String(PyObject *obj);
VALUE Py2RbRaw_Symbol(PyObject *obj);

// Container converters
VALUE Py2Rb_List(PyObject *obj);
VALUE Py2RbRaw_List(PyObject *obj);
VALUE Py2Rb_Dict(PyObject *obj);
VALUE Py2RbRaw_Dict(PyObject *obj);
VALUE Py2Rb_Tuple(PyObject *obj);
VALUE Py2Rb_Set(PyObject *obj);

// Bool Convertor
VALUE Py2Rb_Bool(PyObject *obj);

// Numeric convertors
VALUE Py2Rb_Int(PyObject *obj);
VALUE Py2Rb_Long(PyObject *obj);
VALUE Py2Rb_Float(PyObject *obj);

// Exception Convertor
VALUE Py2Rb_Exception(PyObject *obj);

// ===============================
// Ruby => Python Type Conversions
// ===============================

// Master convertor. Try to convert to other supported types, fall back to Object.
PyObject *Rb2Py_Object(VALUE value);

#define RB2PY(value) \
  Rb2Py_Object(value)

// Master converter. Try to convert to everything else, fall back to object.
PyObject *Rb2Py_Object(VALUE value);
// Container converters
PyObject *Rb2Py_Array(VALUE value);
PyObject *Rb2Py_Hash(VALUE value);
PyObject *Rb2Py_Struct(VALUE value);
// String/Symbol converters
PyObject *Rb2Py_String(VALUE value);
PyObject *Rb2PyRaw_String(VALUE value);
PyObject *Rb2Py_Symbol(VALUE value);
// Boolean converters
PyObject *Rb2Py_Bool(VALUE value);
// Class/Module converters
PyObject *Rb2Py_Class(VALUE value);
PyObject *Rb2Py_Module(VALUE value);
// Numeric Converters
PyObject *Rb2Py_Fixnum(VALUE value);
PyObject *Rb2Py_Bignum(VALUE value);
PyObject *Rb2Py_Float(VALUE value);
// Nil converter
PyObject *Rb2Py_Nil(VALUE value);

void Rb2Py_Error(VALUE value);

// =======================================
// Special-case (or ambiguous) conversions
// =======================================

PyObject *Rb2Py_Tuple(VALUE value);

int convertPyArgs(PyObject *args, VALUE **rb_arg_p);
int convertRbArgs(VALUE args, PyObject **py_arg_p);

#endif // HEADER_CONVERSION_H__INCLUDED
