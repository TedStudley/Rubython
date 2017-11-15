#ifndef HEADER__CONVERSION_H__INCLUDED
#define HEADER__CONVERSION_H__INCLUDED

#include "ruby.h"
#include "Python.h"

#define PY2RB(value) \
  Py2Rb_Object((value))

#define PY2RB_FORWARD(type) \
  VALUE Py2Rb_##type(PyObject *obj);

#define RB_ASSERT_PYTYPE(type) \
  if (!Py##type##_Check(obj)) \
    rb_raise(rb_eTypeError, "Expected Python ##type## type, but got '%s'", \
        Py_Type(obj)->tp_name)

#define PY_ASSERT_RBTYPE(type) \
  if (!RB_TYPE_P(obj, (RUBY_T_##type))) { \
    PyErr_Format(PyExc_TypeError, "Expected Ruby " #type " type, but got '%s'", \
        rb_obj_classname(obj)); \
    return NULL; }

#define PY2RB_CONVERTER(type) \
  VALUE Py2Rb_##type(PyObject *obj) { \
    if (!Py##type##_Check(obj)) \
      rb_raise(rb_eTypeError, "Expected Python ##type## type, but got '%s'", \
          Py_TYPE(obj)->tp_name); \
    else


#define PY2RB_DEFAULT_CONVERSION \
  return rb_cRubython_PyObject__wrap(obj); }

#define PY2RB_WRAP(type) \
  return rb_cRubython_Py##type##__wrap(obj); }

#define PY2RB_CONVERSION(def) \
  return def; }


// Master converter. Try to convert to everything else, fall back to object.
PY2RB_FORWARD(Object);
// Module converter
PY2RB_FORWARD(Module);
// Class converters
PY2RB_FORWARD(Class);
PY2RB_FORWARD(Instance);
// Function converters
PY2RB_FORWARD(Function);
PY2RB_FORWARD(CFunction);
PY2RB_FORWARD(Method);
// Code converter
PY2RB_FORWARD(Code);
// Type converter
PY2RB_FORWARD(Type);
// Bool converter
PY2RB_FORWARD(Bool);
// Container converters
PY2RB_FORWARD(List);
PY2RB_FORWARD(Dict);
PY2RB_FORWARD(Tuple);
PY2RB_FORWARD(Set);
// String convertor
PY2RB_FORWARD(String);
// Numeric convertors
PY2RB_FORWARD(Int);
PY2RB_FORWARD(Long);
PY2RB_FORWARD(Float);

#define CHECK_PYERROR \
  Py2Rb_Error(PyErr_Occurred())

// Only use this to convert error classes
void Py2Rb_Error(PyObject *err);


// ===============================
// Ruby => Python Type Conversions
// ===============================
PyObject *Rb2Py_Object(VALUE value);

#define RB2PY(value) \
  Rb2Py_Object((value))

// Master converter. Try to convert to everything else, fall back to object.
PyObject *Rb2Py_Object(VALUE value);
// Container converters
PyObject *Rb2Py_Array(VALUE value);
PyObject *Rb2Py_Hash(VALUE value);
PyObject *Rb2Py_Struct(VALUE value);
// String/Symbol converters
PyObject *Rb2Py_String(VALUE value);
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
