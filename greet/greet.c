#include <Python.h>

static char greet_method_docs[] = "I return a greeting message";

static PyObject*
greet(PyObject *self)
{
	return Py_BuildValue("s", "Hello, From python extensions world");
}

static PyMethodDef greet_funcs[] = {
	{"greet", (PyCFunction) greet, METH_NOARGS, greet_method_docs},
	{NULL}
};

static char pygreet_module_docs[] = "Hello world module that does nothing";

static struct PyModuleDef greet_module = {
        PyModuleDef_HEAD_INIT,          /* m_base */
        "greet",                        /* m_name */
        pygreet_module_docs,            /* m_doc  */
        -1,                             /* m_size */
        greet_funcs,                    /* m_methods */
        NULL,                           /* m_slots */
        0,                              /* m_traverse */
        0,                              /* m_clear */
        0,                              /* m_free */
};

PyMODINIT_FUNC
PyInit_greet(void)
{
	PyObject *greet = PyModule_Create(&greet_module);
	return greet;
}
