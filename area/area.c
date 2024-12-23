#define PY_SSIZE_T_CLEAN

#include <Python.h>

PyObject* area_exception;

static char area_method_docs[] = "Calculates the area from width and height";

static PyObject*
get_area(PyObject *self, PyObject *args, PyObject *kwds)
{
	double area, width, height = 1;
	char *units = "cm2";
	Py_ssize_t units_strlen;

	char msg[128];

	static char *argnames[] = {"width", "height", "units", NULL};

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "d|ds#", argnames, &width, &height, &units, &units_strlen))
		return NULL;

	area = width * height;

	if (area == 0) {
		PyErr_SetString(area_exception, "Invalid area = 0");
		return NULL;
	}

	sprintf(msg, "%lf%s%s",
		     area,
		     units ? " ": "",
		     units ? units : "");

	return Py_BuildValue("s", msg);
}

static PyMethodDef area_funcs[] = {
	{"get_area", (PyCFunction) get_area, METH_KEYWORDS | METH_VARARGS, area_method_docs},
	{NULL}
};

static char pyarea_module_docs[] = "Hello world module that does nothing";

static struct PyModuleDef area_module = {
        PyModuleDef_HEAD_INIT,          /* m_base */
        "area",                        /* m_name */
        pyarea_module_docs,            /* m_doc  */
        -1,                             /* m_size */
        area_funcs,                    /* m_methods */
        NULL,                           /* m_slots */
        0,                              /* m_traverse */
        0,                              /* m_clear */
        0,                              /* m_free */
};

PyMODINIT_FUNC
PyInit_area(void)
{
	PyObject *area = PyModule_Create(&area_module);

	area_exception = PyErr_NewException("area.AreaException", NULL, NULL);
        PyModule_AddObject(area, "AreaException", area_exception);

	return area;
}
