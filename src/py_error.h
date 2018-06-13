#ifndef HEADER__PY_ERROR_H__INCLUDED
#define HEADER__PY_ERROR_H__INCLUDED

typedef struct {
} py_Rubython_RubythonError;

extern PyObject *py_eRubythonError;
extern PyTypeObject py_Rubython_RubythonError_type;

static int py_eRubython_RubythonError_init(py_Rubython_RubythonError *self, PyObject *args, PyObject *kwds);
static int py_eRubython_RubythonError_clear(py_Rubython_RubythonError *self);
static void py_eRubython_RubythonError_dealloc(PyBaseExceptionObject *self);
static int py_eRubython_RubythonError_traverse(py_Rubython_RubythonError *self, visitproc visit, void *arg);
static PyObject *py_eRubython_RubythonError_str(py_Rubython_RubythonError *self);

void init_Rubython_RubythonError(void);

#endif // HEADER__PY_ERROR_H__INCLUDED
