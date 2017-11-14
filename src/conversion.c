#include "utility.h"
#include "conversion.h"
// Ruby PyTypes
#include "rb_PyObject.h"
#include "rb_PyFunction.h"
#include "rb_PyCFunction.h"
#include "rb_PyType.h"
#include "rb_PyString.h"
// Python RbTypes
#include "py_RbObject.h"
#include "py_RbString.h"
#include "py_RbArray.h"
#include "py_RbHash.h"

// TODO: Do this _WITHOUT_ a gigantic if/else statement. It's not _that_ bad, but it's still pretty bad.
VALUE Py2Rb_Object(PyObject *obj) {
  DEBUG_MARKER;
  VALUE rb_obj = Qundef;

  // TODO: Need to create some sort of PyTypeObject -> Convertor map to give this
  // crap sub linear time complexity in the number of handled types.
  if (PyString_Check(obj))
    rb_obj = Py2Rb_String(obj);
  else if (PyFunction_Check(obj))
    rb_obj = Py2Rb_Function(obj);
  else if (PyCFunction_Check(obj))
    rb_obj = Py2Rb_CFunction(obj);
  else if (PyType_Check(obj))
    rb_obj = Py2Rb_Type(obj);
  // Nil/None
  else if (obj == Py_None)
    rb_obj = Qnil;
  // Bool type
  else if (PyBool_Check(obj))
    rb_obj = Py2Rb_Bool(obj);
  // Numeric types
  else if (PyInt_Check(obj))
    rb_obj = Py2Rb_Int(obj);
  else if (PyLong_Check(obj))
    rb_obj = Py2Rb_Long(obj);
  else if (PyFloat_Check(obj))
    rb_obj = Py2Rb_Float(obj);
  else
    rb_obj = rb_cRubython_PyObject__wrap(obj);

  return rb_obj;
}

// Module Converter
PY2RB_CONVERTER(Module) {
  PY2RB_DEFAULT_CONVERSION;
}

// Class converters
PY2RB_CONVERTER(Class) {
  PY2RB_DEFAULT_CONVERSION;
}

PY2RB_CONVERTER(Instance) {
  PY2RB_DEFAULT_CONVERSION;
}

// Function converters
PY2RB_CONVERTER(Function) {
  PY2RB_WRAP(Function);
}

PY2RB_CONVERTER(CFunction) {
  PY2RB_WRAP(CFunction);
}

PY2RB_CONVERTER(Method) {
  PY2RB_DEFAULT_CONVERSION;
}

// Code converter
PY2RB_CONVERTER(Code) {
  PY2RB_DEFAULT_CONVERSION;
}

// Type converter
PY2RB_CONVERTER(Type) {
  PY2RB_WRAP(Type);
}

// Bool converter
PY2RB_CONVERTER(Bool) {
  PY2RB_CONVERSION((obj == Py_False) ? Qfalse : Qtrue);
}

// Container converters
PY2RB_CONVERTER(List) {
  PY2RB_DEFAULT_CONVERSION;
}
// VALUE Py2Rb_List(PyObject *value) {
//   assert(PyList_Check(value));
//   size_t idx, value_len = PyList_GET_SIZE(value);
//   VALUE rb_value = rb_ary_new2(value_len);
//
//   for (idx = 0; idx < value_len; ++idx)
//     rb_ary_store(rb_value, idx, PY2RB(PyList_GET_ITEM(value, idx)));
//   return rb_value;
// }

PY2RB_CONVERTER(Dict) {
  PY2RB_DEFAULT_CONVERSION;
}
// VALUE Py2Rb_Dict(PyObject *value) {
//   assert(PyDict_Check(value));
//   size_t idx, value_size = ((PyDictObject *)(value))->ma_mask+1;
//
//   VALUE rb_value = rb_hash_new();
//   for (idx = 0; idx < value_size; ++idx) {
//     PyDictEntry it = ((PyDictObject *)(value))->ma_table[idx];
//     if (it.me_key == NULL || it.me_value == NULL)
//       // Only populate entries with values...
//       continue;
//
//     rb_hash_aset(rb_value, PY2RB(it.me_key), PY2RB(it.me_value));
//   }
//   return rb_value;
// }

PY2RB_CONVERTER(Tuple) {
  PY2RB_DEFAULT_CONVERSION;
}
// VALUE Py2Rb_Tuple(PyObject *value) {
//   assert(PyTuple_Check(value));
//   size_t idx, value_len = PyTuple_GET_SIZE(value);
//   VALUE rb_value = rb_ary_new2(value_len);
//
//   for (idx = 0; idx < value_len; ++idx)
//     rb_ary_store(rb_value, idx, PY2RB(PyTuple_GET_ITEM(value, idx)));
//   return rb_value;
// }

PY2RB_CONVERTER(Set) {
  PY2RB_DEFAULT_CONVERSION;
}
// VALUE Py2Rb_Set(PyObject *value) {
//   assert(PySet_Check(value));
//   size_t idx, value_size = PySet_GET_SIZE(value);
//
//   VALUE cSet = rb_const_get(rb_cObject, rb_intern("Set"));
//   VALUE rb_value = rb_funcall(cSet, rb_intern("new"), 0);
//   for (idx = 0; idx < value_size; ++idx) {
//     PyObject *it = ((PySetObject *)(value))->table[idx].key;
//     rb_funcall(rb_value, rb_intern("<<"), 1, PY2RB(it));
//   }
//   return rb_value;
// }

// String converter
PY2RB_CONVERTER(String) {
  PY2RB_WRAP(String);
}

// Numeric convertors
PY2RB_CONVERTER(Int) {
  PY2RB_CONVERSION(LONG2NUM(PyInt_AS_LONG(obj)));
}

PY2RB_CONVERTER(Long) {
  PY2RB_CONVERSION(LONG2NUM(PyLong_AsLong(obj)));
}

PY2RB_CONVERTER(Float) {
  PY2RB_CONVERSION(DBL2NUM(PyFloat_AS_DOUBLE(obj)));
}

void Py2Rb_Error(PyObject *err) {
  fprintf(stderr, "current python error: %s\n", PyExceptionClass_Name(err));
  fprintf(stderr, "err: %p\n", err);
  fprintf(stderr, "err->dict: %p\n", ((PyBaseExceptionObject *)(err))->dict);
  fprintf(stderr, "err->args: %p\n", ((PyBaseExceptionObject *)(err))->args);
  fprintf(stderr, "err->message: %p\n", ((PyBaseExceptionObject *)(err))->message);
  if (err == NULL)
    return;
  rb_raise(rb_eRuntimeError, "Caught Python error %s: %s",
      PyExceptionClass_Name(err),
      PyString_AS_STRING(((PyBaseExceptionObject *)(err))->message));
}

PyObject *Rb2Py_Object(VALUE obj) {
  PyObject *py_obj = NULL;
  switch (TYPE(obj)) {
  // Check for a PyObject wrapper first
    case RUBY_T_DATA:
      if (RTYPEDDATA_P(obj) && rb_typeddata_is_kind_of(obj, &rb_Rubython_PyObject_type))
        TypedData_Get_Struct(obj, rb_Rubython_PyObject, &rb_Rubython_PyObject_type, py_obj);
      else
        py_obj = RbObjectWrap(obj);
      break;
  // Container convertors
  case RUBY_T_ARRAY:
    py_obj = Rb2Py_Array(obj);
    break;
  case RUBY_T_HASH:
    py_obj = Rb2Py_Hash(obj);
    break;
  case RUBY_T_STRUCT:
    py_obj = Rb2Py_Struct(obj);
    break;
  // String/symbol convertors
  case RUBY_T_STRING:
    py_obj = Rb2Py_String(obj);
    break;
  case RUBY_T_SYMBOL:
    py_obj = Rb2Py_Symbol(obj);
    break;
  // Boolean convertor
  case RUBY_T_TRUE:
  case RUBY_T_FALSE:
    py_obj = Rb2Py_Bool(obj);
    break;
  // Class/module convertors
  case RUBY_T_CLASS:
    py_obj = Rb2Py_Class(obj);
    break;
  case RUBY_T_MODULE:
    py_obj = Rb2Py_Module(obj);
    break;
  // Numeric convertors
  case RUBY_T_FIXNUM:
    py_obj = Rb2Py_Fixnum(obj);
    break;
  case RUBY_T_BIGNUM:
    py_obj = Rb2Py_Bignum(obj);
    break;
  case RUBY_T_FLOAT:
    py_obj = Rb2Py_Float(obj);
    break;
  // Nil convertor
  case RUBY_T_NIL:
    py_obj = Rb2Py_Nil(obj);
    break;
  default:
    fprintf(stderr, "Unhandled Ruby type: %s\n", rb_obj_classname(obj));
    py_obj = RbObjectWrap(obj);
    break;
  }
  Py_RETURN(py_obj);
}

PyObject *Rb2Py_Array(VALUE obj) {
  DEBUG_MARKER;
  PY_ASSERT_RBTYPE(ARRAY);
  return RbArrayWrap(obj);
}

PyObject *Rb2Py_Hash(VALUE obj) {
  DEBUG_MARKER;
  PY_ASSERT_RBTYPE(HASH);
  return RbHashWrap(obj);
}

PyObject *Rb2Py_Struct(VALUE obj) {
  DEBUG_MARKER;
  PY_ASSERT_RBTYPE(STRUCT);
  // TODO: return RbStructWrap(obj);
  return RbObjectWrap(obj);
}

PyObject *Rb2Py_String(VALUE obj) {
  DEBUG_MARKER;
  PY_ASSERT_RBTYPE(STRING);
  return RbStringWrap(obj);
}

PyObject *Rb2Py_Symbol(VALUE obj) {
  DEBUG_MARKER;
  PY_ASSERT_RBTYPE(SYMBOL);
  // TODO: return RbSymbolWrap(obj);
  return RbObjectWrap(obj);
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
  return RbObjectWrap(obj);
}

PyObject *Rb2Py_Module(VALUE obj) {
  DEBUG_MARKER;
  PY_ASSERT_RBTYPE(MODULE);
  // TODO: return RbModuleWrap(obj);
  return RbObjectWrap(obj);
}

PyObject *Rb2Py_Fixnum(VALUE obj) {
  DEBUG_MARKER;
  PY_ASSERT_RBTYPE(FIXNUM);
  return PyInt_FromLong(FIX2LONG(obj));
}

PyObject *Rb2Py_Bignum(VALUE obj) {
  DEBUG_MARKER;
  PY_ASSERT_RBTYPE(BIGNUM);
  // TODO: ???
  Py_RETURN_NONE;
}

PyObject *Rb2Py_Float(VALUE obj) {
  DEBUG_MARKER;
  PY_ASSERT_RBTYPE(FLOAT);
  return PyFloat_FromDouble(NUM2DBL(obj));
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
  Py_INCREF(py_tuple);

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
