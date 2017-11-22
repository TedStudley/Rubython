#include "ruby.h"
#include "Python.h"

#include "utility.h"
#include "conversion.h"
#include "rb_PyTypes.h"
#include "rb_PyObject.h"
#include "rb_PyString.h"
#include "rb_PyContext.h"

VALUE rb_cPyObject;


static void rb_cRubython_PyObject__mark(void *ptr) {
  DEBUG_MARKER;
}

static void rb_cRubython_PyObject__free(void *ptr) {
  DEBUG_MARKER;
  Py_DECREF(ptr);
}

static size_t rb_cRubython_PyObject__memsize(const void *ptr) {
  DEBUG_MARKER;
  const rb_Rubython_PyObject *py_object = ptr;

  // TODO: I don't think this actually works properly
  return sizeof(py_object) + Py_SIZE(py_object);
}

const rb_data_type_t rb_Rubython_PyObject_type = {
  "Rubython/PyObject",
  {rb_cRubython_PyObject__mark, rb_cRubython_PyObject__free, rb_cRubython_PyObject__memsize,},
  0, 0,
  RUBY_TYPED_FREE_IMMEDIATELY
};

VALUE PyObject_WRAP(PyObject *ptr) {
  DEBUG_MARKER;
  Py_INCREF(ptr);

  VALUE self = TypedData_Wrap_Struct(rb_cPyObject, &rb_Rubython_PyObject_type, ptr);
  rb_funcall(self, rb_intern("initialize"), 0);
  return self;
};

static VALUE
rb_cRubython_PyObject_initialize(VALUE self) {
  DEBUG_MARKER;
  VALUE context;
  context = rb_funcall(rb_cPyContext, rb_intern("get_instance"), 0);

  // TODO: This should be a struct-level variable. it should _never_ be
  // possible to change it from within Ruby/Python
  rb_ivar_set(self, rb_intern("@context"), context);
  return self;
}

VALUE rb_cRubython_PyObject_context(VALUE self) {
  DEBUG_MARKER;

  return rb_ivar_get(self, rb_intern("@context"));
}

VALUE rb_cRubython_PyObject_is_validq(VALUE self) {
  DEBUG_MARKER;
  // TODO: This should be a struct-level variable. It should _never_ be
  // possible to change it from within Ruby/Python
  VALUE py_context = rb_ivar_get(self, rb_intern("@context"));

  return rb_funcall(py_context, rb_intern("is_valid?"), 0);
}


VALUE rb_cRubython_PyObject_to_s(VALUE self) {
  DEBUG_MARKER;
  return Py2RbRaw_String(PyObject_Str(PyObject_PTR(self)));
}

VALUE rb_cRubython_PyObject_inspect(VALUE self) {
  DEBUG_MARKER;
  return Py2RbRaw_String(PyObject_Repr(PyObject_PTR(self)));
}

VALUE rb_cRubython_PyObject_hasattr(VALUE self, VALUE rb_key) {
  DEBUG_MARKER;
  const char *key;
  if (RB_TYPE_P(rb_key, T_STRING)) {
    key = RSTRING_PTR(rb_key);
  } else if (SYMBOL_P(rb_key)) {
    key = rb_id2name(SYM2ID(rb_key));
  } else {
    rb_raise(rb_eTypeError, "Expected string or symbol");
  }

  return (PyObject_HasAttrString(PyObject_PTR(self), key) ? Qtrue : Qfalse);
}

VALUE rb_cRubython_PyObject_getattr(VALUE self, VALUE rb_key) {
  DEBUG_MARKER;
  PyObject *py_attr;
  const char *key;
  if (RB_TYPE_P(rb_key, T_STRING)) {
    key = RSTRING_PTR(rb_key);
  } else if (SYMBOL_P(rb_key)) {
    key = rb_id2name(SYM2ID(rb_key));
  } else
    rb_raise(rb_eTypeError, "Expected string or symbol");

  py_attr = PyObject_GetAttrString(PyObject_PTR(self), key);
  if (py_attr == NULL) {
    DEBUG_MSG("TODO: Better Errors");
    HandlePyErrors("Recieved Error from __getattr__");
    assert(0); // If we got here, something is funky.
  }

  return PY2RB(py_attr);
}

VALUE rb_cRubython_PyObject_setattr(VALUE self, VALUE rb_key, VALUE rb_attr) {
  DEBUG_MARKER;
  const char *key;
  int result = 0;
  if (RB_TYPE_P(rb_key, T_STRING)) {
    key = RSTRING_PTR(rb_key);
  } else if (SYMBOL_P(rb_key)) {
    key = rb_id2name(SYM2ID(rb_key));
  } else {
    rb_raise(rb_eTypeError, "Expected string or symbol");
  }

  result = PyObject_SetAttrString(PyObject_PTR(self), key, RB2PY(rb_attr));
  if (result != 0) {
    DEBUG_MSG("TODO: Better Errors");
    HandlePyErrors("Encountered Error from __setattr__");
    assert(0); // If we got here, something is funky.
  }
  return Qnil;
}

VALUE rb_cRubython_PyObject_callableq(VALUE self) {
  DEBUG_MARKER;
  return PyCallable_Check(PyObject_PTR(self)) ? Qtrue : Qfalse;
}

VALUE
rb_cRubython_PyObject_call(VALUE self, VALUE args) {
  DEBUG_MARKER;
  PyObject *py_result, *py_args;
  VALUE result = Qundef;

  convertRbArgs(args, &py_args);
  // TODO: Support kwargs
  py_result = PyObject_Call(PyObject_PTR(self), py_args, NULL);
  if (py_result == NULL) {
    HandlePyErrors("TODO: Better Errors!");
    assert(0); // If we got here, something is funky
  }

  return PY2RB(py_result);
}

VALUE rb_cRubython_PyObject_method_missing(int argc, VALUE *argv, VALUE self) {
  DEBUG_MARKER;
  VALUE mid, org_mid, sym, v;
  PyObject *py_attr;
  const char *mname;

  rb_check_arity(argc, 1, UNLIMITED_ARGUMENTS);
  mid = org_mid = argv[0];
  sym = rb_check_symbol(&mid);
  if (!NIL_P(sym)) mid = rb_sym2str(sym);
  mname = StringValueCStr(mid);
  if (!mname) {
    DEBUG_MSG("TODO: Better Errors!");
    rb_raise(rb_eRuntimeError, "Fail: Unknown method or property");
  }

  if (!PyObject_HasAttrString(PyObject_PTR(self), mname))
    return rb_call_super(argc, argv);

  py_attr = PyObject_GetAttrString(PyObject_PTR(self), mname);
  if (py_attr == NULL) {
    HandlePyErrors("Error attempting to load Python name");
    // Not sure if this will ever happen.
    return rb_call_super(argc, argv);
  }

  if (PyCallable_Check(py_attr) && !PyType_Check(py_attr)) {
    VALUE rb_args = rb_ary_new_from_values(argc - 1, argv + 1);
    PyObject *py_args;
    convertRbArgs(rb_args, &py_args);
    // TODO: Support keyword args
    py_attr = PyObject_Call(py_attr, py_args, NULL);
    if (py_attr == NULL) {
      HandlePyErrors("TODO: Handle Errors!");
      return Qundef;
    }
  }

  return PY2RB(py_attr);
}

// TODO: Implement 'to_python' for core types (String, Array, Hash)

void Init_PyObject(void) {
  DEBUG_MARKER;

  rb_cPyObject = rb_define_class_under(rb_mPyTypes, "PyObject", rb_cObject);
  rb_undef_alloc_func(rb_cPyObject);

  rb_define_method(rb_cPyObject, "initialize", rb_cRubython_PyObject_initialize, 0);

  rb_define_private_method(rb_cPyObject, "method_missing", rb_cRubython_PyObject_method_missing, -1);

  rb_define_method(rb_cPyObject, "context", rb_cRubython_PyObject_context, 0);
  rb_define_method(rb_cPyObject, "is_valid?", rb_cRubython_PyObject_is_validq, 0);

  rb_define_method(rb_cPyObject, "to_s", rb_cRubython_PyObject_to_s, 0);
  rb_define_method(rb_cPyObject, "inspect", rb_cRubython_PyObject_inspect, 0);

  rb_define_method(rb_cPyObject, "hasattr", rb_cRubython_PyObject_hasattr, 1);
  rb_define_method(rb_cPyObject, "getattr", rb_cRubython_PyObject_getattr, 1);

  rb_define_method(rb_cPyObject, "kind_of?", rb_obj_is_kind_of, 1);

  rb_define_method(rb_cPyObject, "call", rb_cRubython_PyObject_call, -2);
}
