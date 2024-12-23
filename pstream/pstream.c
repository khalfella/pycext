#include <Python.h>
#include <stdbool.h>


struct pstream {
	uint64_t prime;
};

static bool pstream_is_prime(uint64_t num) {
	uint64_t i;

	for (i = 2; i < num; i++) {
		if (num % i == 0) return false;
	}
	return true;
}

static int pstream_init(struct pstream *ps, uint64_t start) {
	while (!pstream_is_prime(start))
		start++;

	ps->prime = start;
	return 0;
}

static uint64_t pstream_get(struct pstream *ps) {
	uint64_t prime = ps->prime;

	ps->prime++;
	while (!pstream_is_prime(ps->prime))
		ps->prime++;
	return prime;
}

struct pypstream {
	PyObject_HEAD
	struct pstream ps;
};

PyObject* pypstream_exception;

/* pstream.PrimeStream.__init__() */
static int pypstream_init(PyObject *self, PyObject *args, PyObject *kwds) {
	struct pypstream *pyps = (struct pypstream*) self;
	uint64_t start = 2;
	int ret;

	static char *argnames[] = {"start", NULL};

	ret = PyArg_ParseTupleAndKeywords(args, kwds, "|K", argnames, &start);
	if (!ret) {
		/* Exception has already been raised */
		return -1;
	}

	ret = pstream_init(&pyps->ps, start);
	if (ret) {
		PyErr_Format(pypstream_exception, "Failed to initialize PrimeStream: %d", ret);
		return -1;
	}
	return 0;
}

/* To be deallocated */
static void pypstream_dealloc(PyObject *self) {
}

/* pstream.PrimeStream.__repr__() */
static PyObject* pypstream_str(PyObject *self) {
	return Py_BuildValue("");
}

static PyObject* pypstream_get(PyObject *self) {
	struct pypstream *pyps = (struct pypstream*) self;
	uint64_t prime = pstream_get(&pyps->ps);
	return Py_BuildValue("K", prime);
}

static PyMethodDef pypstream_methods[] = {
	{
		"get",
		(PyCFunction)pypstream_get,
		METH_NOARGS,
		"Returns next prime number"
	},
	{NULL}
};

static PyTypeObject t_pstream = {
	PyVarObject_HEAD_INIT(NULL, 0)		/* tp_head */
	.tp_name = "pstream.PrimeStream",
	.tp_basicsize = sizeof(struct pypstream),
	.tp_dealloc = pypstream_dealloc,
	.tp_repr = pypstream_str,
	.tp_flags = Py_TPFLAGS_DEFAULT,
	.tp_doc = "Prime Stream Generator",
	.tp_methods = pypstream_methods,
	.tp_init = pypstream_init,
	.tp_alloc = PyType_GenericAlloc,
	.tp_new = PyType_GenericNew,
	.tp_free = PyObject_Del,
};

static char pypstream_module_docs[] = "Hello, From Python extension world";

static struct PyModuleDef pstream_module = {
        PyModuleDef_HEAD_INIT,          /* m_base */
        "pstream",                        /* m_name */
        pypstream_module_docs,            /* m_doc  */
        -1,                             /* m_size */
        NULL,                           /* m_methods */
        NULL,                           /* m_slots */
        0,                              /* m_traverse */
        0,                              /* m_clear */
        0,                              /* m_free */
};

PyMODINIT_FUNC
PyInit_pstream(void)
{
	PyObject *pstream = PyModule_Create(&pstream_module);

	PyType_Ready(&t_pstream);
	PyObject_SetAttrString(pstream, "PrimeStream", (PyObject*)&t_pstream);

	pypstream_exception = PyErr_NewException("pstream.PrimeStreamException", NULL, NULL);
	PyModule_AddObject(pstream, "PrimeStreamException", pypstream_exception);

	return pstream;
}
