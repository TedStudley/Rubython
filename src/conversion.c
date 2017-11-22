#include "utility.h"
#include "conversion.h"
// Ruby PyTypes
#include "rb_PyObject.h"
#include "rb_PyString.h"
#include "rb_PyList.h"
#include "rb_PyDict.h"
// Python RbTypes
#include "py_RbObject.h"
#include "py_RbString.h"
#include "py_RbArray.h"
#include "py_RbHash.h"

// Constant Py2Rb definitions
#define Py2Rb_BOOL(obj)  (((PyObject *)(obj) == Py_True) ? Qtrue : Qfalse)
#define Py2Rb_INT(obj)   LONG2NUM(PyInt_AS_LONG((PyObject *)(obj)))
#define Py2Rb_LONG(obj)  LONG2NUM(PyLong_AsLong((PyObject *)(obj)))
#define Py2Rb_FLOAT(obj) DBL2NUM(PyFloat_AS_DOUBLE(obj))

VALUE Py2Rb_Object(PyObject *obj) {
  DEBUG_MARKER;
  VALUE rb_obj = Qundef;

  // TODO: Need to create some sort of PyTypeObject -> Convertor map to give this
  // crap sub linear time complexity in the number of handled types.
  if (PyObject_IsInstance(obj, (PyObject *)&py_Rubython_RbObject_type)) {
    rb_obj = ((py_Rubython_RbObject *)(obj))->as.value;
  } else if (PyString_Check(obj)) {
    rb_obj = PyString_WRAP(obj);
  } else if (PyDict_Check(obj)) {
    rb_obj = PyDict_WRAP(obj);
  } else if (PyList_Check(obj)) {
    rb_obj = PyList_WRAP(obj);
  // Nil/None
  } else if (obj == Py_None) {
    rb_obj = Qnil;
  // Bool type
  } else if (PyBool_Check(obj)) {
    rb_obj = Py2Rb_BOOL(obj);
  // Numeric types
  } else if (PyInt_Check(obj)) {
    rb_obj = Py2Rb_INT(obj);
  } else if (PyLong_Check(obj)) {
    rb_obj = Py2Rb_LONG(obj);
  } else if (PyFloat_Check(obj)) {
    rb_obj = Py2Rb_FLOAT(obj);
  } else {
    rb_obj = PyObject_WRAP(obj);
  }

  return rb_obj;
}

#define RbAssertPyType(obj, type) \
  if (!Py##type##_Check(obj)) \
    rb_raise(rb_eTypeError, "Expected Python ##type## type, but got %s", \
        Py_TYPE(obj)->tp_name)

// String converter
inline VALUE Py2Rb_String(PyObject *obj) {
  DEBUG_MARKER;
  RbAssertPyType(obj, String);
  return PyString_WRAP(obj);
}

inline VALUE Py2RbRaw_String(PyObject *obj) {
  DEBUG_MARKER;
  RbAssertPyType(obj, String);
  return rb_str_new_cstr(PyString_AS_STRING(obj));
}

inline VALUE Py2RbRaw_Symbol(PyObject *obj) {
  DEBUG_MARKER;
  RbAssertPyType(obj, String);
  return ID2SYM(rb_intern2(PyString_AS_STRING(obj), PyString_Size(obj)));
}

// Bool converter
VALUE Py2Rb_Bool(PyObject *obj) {
  DEBUG_MARKER;
  RbAssertPyType(obj, Bool);
  return Py2Rb_BOOL(obj);
}

// Container converters
VALUE Py2Rb_List(PyObject *obj) {
  DEBUG_MARKER;
  RbAssertPyType(obj, List);
  return PyList_WRAP(obj);
}

VALUE Py2RbRaw_List(PyObject *obj) {
  DEBUG_MARKER;
  RbAssertPyType(obj, List);
  VALUE rb_ary;
  Py_ssize_t py_len;

  py_len = PyList_Size(obj);
  rb_ary = rb_ary_new2(py_len);
  for (Py_ssize_t idx = 0; idx < py_len; ++idx)
    rb_ary_store(rb_ary, idx, PY2RB(PyList_GetItem(obj, idx)));

  return rb_ary;
}

VALUE Py2Rb_Dict(PyObject *obj) {
  DEBUG_MARKER;
  RbAssertPyType(obj, Dict);
  return PyDict_WRAP(obj);
}

VALUE Py2RbRaw_Dict(PyObject *obj) {
  DEBUG_MARKER;
  return Qundef;
}

// Numeric convertors
VALUE Py2Rb_Int(PyObject *obj) {
  DEBUG_MARKER;
  RbAssertPyType(obj, Int);
  return Py2Rb_INT(obj);
}

VALUE Py2Rb_Long(PyObject *obj) {
  DEBUG_MARKER;
  RbAssertPyType(obj, Long);
  return Py2Rb_LONG(obj);
}

VALUE Py2Rb_Float(PyObject *obj) {
  DEBUG_MARKER;
  RbAssertPyType(obj, Float);
  return Py2Rb_FLOAT(obj);
}

VALUE Py2Rb_Exception(PyObject *obj) {
  DEBUG_MARKER;
  RbAssertPyType(obj, ExceptionInstance);
  return PyObject_WRAP(obj);
}

// ======================
// ======================
//     RUBY => PYTHON
// ======================
// ======================

// Constant Rb2Py definitions
#define Rb2Py_BOOL(obj)  (((PyObject *)(obj) == Py_True) ? Qtrue : Qfalse)
#define Rb2Py_INT(obj)   LONG2NUM(PyInt_AS_LONG((PyObject *)(obj)))
#define Rb2Py_LONG(obj)  LONG2NUM(PyLong_AsLong((PyObject *)(obj)))
#define Rb2Py_FLOAT(obj) DBL2NUM(PyFloat_AS_DOUBLE(obj))

PyObject *Rb2Py_Object(VALUE obj) {
  PyObject *py_obj = NULL;
  switch (TYPE(obj)) {
  // Check for a PyObject wrapper first
    case RUBY_T_DATA:
      DEBUG_PRINTF("T_DATA");
      if (RTYPEDDATA_P(obj) && rb_typeddata_is_kind_of(obj, &rb_Rubython_PyObject_type)) {
        DEBUG_PRINTF("rb_cPyObject");
        TypedData_Get_Struct(obj, rb_Rubython_PyObject, &rb_Rubython_PyObject_type, py_obj);
      } else {
        DEBUG_PRINTF("RObject");
        py_obj = RbObject_WRAP(obj);
      }
      break;
  // String/symbol convertors
  case RUBY_T_STRING:
    DEBUG_PRINTF("T_STRING");
    py_obj = RbString_WRAP(obj);
    break;
  case RUBY_T_SYMBOL:
    DEBUG_PRINTF("T_SYMBOL");
    py_obj = Rb2Py_Symbol(obj);
    break;
  // Container convertors
  case RUBY_T_ARRAY:
    DEBUG_PRINTF("T_ARRAY");
    py_obj = Rb2Py_Array(obj);
    break;
  case RUBY_T_HASH:
    DEBUG_PRINTF("T_HASH");
    py_obj = Rb2Py_Hash(obj);
    break;
  // Boolean convertor
  case RUBY_T_TRUE:
  case RUBY_T_FALSE:
    DEBUG_PRINTF("T_TRUE/T_FALSE");
    py_obj = Rb2Py_Bool(obj);
    break;
  // Numeric convertors
  case RUBY_T_FIXNUM:
    DEBUG_PRINTF("T_FIXNUM");
    py_obj = Rb2Py_Fixnum(obj);
    break;
  case RUBY_T_BIGNUM:
    DEBUG_PRINTF("T_BIGNUM");
    py_obj = Rb2Py_Bignum(obj);
    break;
  case RUBY_T_FLOAT:
    DEBUG_PRINTF("T_FLOAT");
    py_obj = Rb2Py_Float(obj);
    break;
  // Nil convertor
  case RUBY_T_NIL:
    DEBUG_PRINTF("T_NIL");
    py_obj = Rb2Py_Nil(obj);
    break;
  default:
    DEBUG_PRINTF("%s", rb_obj_classname(obj));
    py_obj = RbObject_WRAP(obj);
    break;
  }
  return py_obj;
}

PyObject *Rb2Py_Array(VALUE obj) {
  DEBUG_MARKER;
  PY_ASSERT_RBTYPE(ARRAY);
  return RbArray_WRAP(obj);
}

PyObject *Rb2Py_Hash(VALUE obj) {
  DEBUG_MARKER;
  PY_ASSERT_RBTYPE(HASH);
  return RbHash_WRAP(obj);
}

PyObject *Rb2Py_Struct(VALUE obj) {
  DEBUG_MARKER;
  PY_ASSERT_RBTYPE(STRUCT);
  // TODO: return RbStructWrap(obj);
  return RbObject_WRAP(obj);
}

PyObject *Rb2Py_String(VALUE obj) {
  DEBUG_MARKER;
  PY_ASSERT_RBTYPE(STRING);
  return RbString_WRAP(obj);
}

PyObject *Rb2PyRaw_String(VALUE obj) {
  DEBUG_MARKER;
  PY_ASSERT_RBTYPE(STRING);
  return PyString_FromString(StringValueCStr(obj));
}

PyObject *Rb2Py_Symbol(VALUE obj) {
  DEBUG_MARKER;
  PY_ASSERT_RBTYPE(SYMBOL);
  // TODO: return RbSymbolWrap(obj);
  return RbObject_WRAP(obj);
}

PyObject *Rb2Py_Bool(VALUE obj) {
  DEBUG_MARKER;
  switch (TYPE(obj)) {
  case RUBY_T_TRUE:
    return Py_True;
  case RUBY_T_FALSE:
    return Py_False;
  }
  PyErr_Format(PyExc_TypeError, "Expected Ruby TRUE/FALSE type, but got '%s'", \
      rb_obj_classname(obj));
  return NULL;
}

PyObject *Rb2Py_Class(VALUE obj) {
  DEBUG_MARKER;
  PY_ASSERT_RBTYPE(CLASS);
  // TODO: return RbClassWrap(obj);
  return RbObject_WRAP(obj);
}

PyObject *Rb2Py_Module(VALUE obj) {
  DEBUG_MARKER;
  PY_ASSERT_RBTYPE(MODULE);
  // TODO: return RbModuleWrap(obj);
  return RbObject_WRAP(obj);
}

PyObject *Rb2Py_Fixnum(VALUE obj) {
  DEBUG_MARKER;
  PY_ASSERT_RBTYPE(FIXNUM);
  return PyInt_FromLong(FIX2LONG(obj));
}

PyObject *Rb2Py_Bignum(VALUE obj) {
  DEBUG_MARKER;
  PY_ASSERT_RBTYPE(BIGNUM);
  return PyLong_FromLong(NUM2ULONG(obj));
}

PyObject *Rb2Py_Float(VALUE obj) {
  DEBUG_MARKER;
  // TODO: RB_TYPE_P is _broken_ for T_FLOAT due to redefinition by Python
  if (!RB_FLOAT_TYPE_P(obj)) {
    PyErr_Format(PyExc_TypeError, "Expected Ruby T_FLOAT type, but got '%s'",
        rb_obj_classname(obj));
    return NULL;
  }
  PyObject *py_float = PyFloat_FromDouble(NUM2DBL(obj));
  return py_float;
}

PyObject *Rb2Py_Nil(VALUE obj) {
  DEBUG_MARKER;
  PY_ASSERT_RBTYPE(NIL);
  return Py_None;
}

// Explicit Ambiguous Conversions
PyObject *Rb2Py_Tuple(VALUE obj) {
  // TODO: ???
  size_t len = RARRAY_LEN(obj);
  PyObject *py_tuple = PyTuple_New(len);

  for (size_t idx = 0; idx < len; ++idx) {
    VALUE rb_entry = RARRAY_AREF(obj, idx);
    if (!PyTuple_SetItem(py_tuple, idx, RB2PY(rb_entry)))
      rb_raise(rb_eRuntimeError, "Error while setting Tuple value");
  }

  return py_tuple;
}

int convertPyArgs(PyObject *argv, VALUE **rb_argv_p) {
  DEBUG_MARKER;
  if (!PyTuple_CheckExact(argv)) {
    DEBUG_MSG("TODO: Handle Errors");
    return -1;
  }

  int arglen = (int)PyTuple_Size(argv);
  (*rb_argv_p) = (VALUE *)malloc(sizeof(VALUE)*arglen);

  for (int idx = 0; idx < arglen; ++idx)
    (*rb_argv_p)[idx] = PY2RB(PyTuple_GetItem(argv, idx));
  return arglen;
}

int convertRbArgs(VALUE argv, PyObject **py_argv_p) {
  DEBUG_MARKER;
  Check_Type(argv, T_ARRAY);

  int arglen = (int)RARRAY_LEN(argv);
  (*py_argv_p) = PyTuple_New(arglen);

  for (int idx = 0; idx < arglen; ++idx)
    PyTuple_SetItem((*py_argv_p), idx, RB2PY(RARRAY_AREF(argv, idx)));
  return arglen;
}
