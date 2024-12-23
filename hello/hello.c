#include <Python.h>


static char pyhello_module_docs[] = "Hello, From Python extension world";

static struct PyModuleDef hello_module = {
        PyModuleDef_HEAD_INIT,          /* m_base */
        "hello",                        /* m_name */
        pyhello_module_docs,            /* m_doc  */
        -1,                             /* m_size */
        NULL,                           /* m_methods */
        NULL,                           /* m_slots */
        0,                              /* m_traverse */
        0,                              /* m_clear */
        0,                              /* m_free */
};

PyMODINIT_FUNC
PyInit_hello(void)
{
	PyObject *hello = PyModule_Create(&hello_module);
	return hello;
}
