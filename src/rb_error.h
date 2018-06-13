#ifndef HEADER__RB_ERROR_H__INCLUDED
#define HEADER__RB_ERROR_H__INCLUDED

extern VALUE rb_eRubythonError;

void rb_rubython_error(const char *fmt, ...);
void rb_rubython_py_error(VALUE py_error);
void rb_rubython_py_error_str(VALUE py_error, const char *fmt, ...);

VALUE rb_eRubython_RubythonError_initialize(int argc, VALUE *argv, VALUE self);

void Init_RubythonError(void);

#endif // HEADER__RB_ERROR_H__INCLUDED
