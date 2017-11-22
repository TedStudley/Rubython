#include "ruby.h"

#include "utility.h"
#include "rb_error.h"
#include "rb_Rubython.h"
#include "rb_PyTypes.h"
#include "rb_PyContext.h"

VALUE rb_mRubython;

/*
 * Document-module: Rubython
 *
 * A Ruby/Python Interoperability Gem.
 */
void Init_rubython_ext() {
  DEBUG_MARKER;
  rb_mRubython = rb_define_module("Rubython");

  Init_PyTypes();
  Init_PyContext();
  Init_RubythonError();
}
