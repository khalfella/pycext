#define PY_SSIZE_T_CLEAN

#include <Python.h>

static char salute_method_docs[] = "I return a salute message";

static PyObject*
salute(PyObject *self, PyObject *args)
{
	char *first_name = NULL, *last_name = NULL;
	Py_ssize_t first_name_length, last_name_length;
	char msg[128];

	if (!PyArg_ParseTuple(args, "s#|s#", &first_name, &first_name_length, &last_name, &last_name_length))
		return NULL;


	sprintf(msg, "Hello %s%s%s, From python extensions",
		     first_name,
		     last_name ? " ": "",
		     last_name ? last_name : "");

	return Py_BuildValue("s", msg);
}

static PyMethodDef salute_funcs[] = {
	{"salute", (PyCFunction) salute, METH_KEYWORDS | METH_VARARGS, salute_method_docs},
	{NULL}
};

static char pysalute_module_docs[] = "Hello world module that does nothing";

static struct PyModuleDef salute_module = {
        PyModuleDef_HEAD_INIT,          /* m_base */
        "salute",                        /* m_name */
        pysalute_module_docs,            /* m_doc  */
        -1,                             /* m_size */
        salute_funcs,                    /* m_methods */
        NULL,                           /* m_slots */
        0,                              /* m_traverse */
        0,                              /* m_clear */
        0,                              /* m_free */
};

PyMODINIT_FUNC
PyInit_salute(void)
{
	PyObject *salute = PyModule_Create(&salute_module);
	return salute;
}
