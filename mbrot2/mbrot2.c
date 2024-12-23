#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


struct thread_info {
	pthread_t		thread;
	unsigned int		width;		/* Image width  */
	unsigned int		height;		/* Image height */
	double			x0;
	double			y0;
	double			x1;
	double			y1;
	char			*buffer;
};

struct mbrot {
	unsigned int		width;		/* Image width  */
	unsigned int		height;		/* Image height */
	double			x0;
	double			y0;
	double			x1;
	double			y1;
	unsigned int		nthreads;

	char			*buffer;
	struct thread_info	*tinfo;
};

static unsigned char mbrot_calc(double x, double y) {
	unsigned char iter = 0;

	double u  = 0, v  = 0;
	double u2 = 0, v2 = 0;

	while (u2 + v2 < 4.0 && iter < 255) {
		v = 2 * u * v + y;
		u = u2 - v2 + x;
		u2 = u * u;
		v2 = v * v;
		iter++;
	}

	return iter;
}

static int mbrot_fill_buffer(char *buffer, unsigned int width,
				  unsigned int height, double x0, double y0,
				  double x1, double y1) {

	double xdelta = (x1 - x0) / ((double) width);
	double ydelta = (y1 - y0) / ((double) height);
	unsigned int w, h;
	double x, y;

	y = y0;
	for (h = 0; h < height; h++, y += ydelta) {
		x = x0;
		for (w = 0; w < width; w++, x += xdelta) {
			buffer[h * width + w] = mbrot_calc(x, y);
		}
	}
}

static char *mbrot_get_buffer(struct mbrot *mb, size_t *sz) {
	*sz = mb->width * mb->height;
	return mb->buffer;
}

void *mbrot2_thread_func(void *arg) {
	struct thread_info *ti = arg;
	mbrot_fill_buffer(ti->buffer, ti->width, ti->height,
			       ti->x0, ti->y0, ti->x1, ti->y1);
	return NULL;
}

static int mbrot_init(struct mbrot *mb, unsigned int width,
			   unsigned int height, double x0, double y0,
			   double x1, double y1, unsigned int nthreads) {

	mb->width = width;
	mb->height = height;
	mb->x0 = x0;
	mb->y0 = y0;
	mb->x1 = x1;
	mb->y1 = y1;

	mb->nthreads = nthreads;

	mb->buffer = malloc(width * height);
	if (mb->buffer == NULL) {
		return 1;
	}

	if (nthreads == 0) {
		mb->tinfo = NULL;
		mbrot_fill_buffer(mb->buffer, width, height,
				       x0, y0, x1, y1);
		return 0;
	}

	mb->tinfo = malloc(sizeof(struct thread_info) * nthreads);
	if (mb->tinfo == NULL) {
		free(mb->buffer);
		mb->buffer = NULL;
		return 1;
	}

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))
	unsigned int chunk = DIV_ROUND_UP(height, nthreads);

	double xdelta = (x1 - x0) / ((double) width);
	double ydelta = (y1 - y0) / ((double) height);
	double tx0 = x0;
	double ty0 = y0;

	unsigned int remain = height;
	char *buffer = mb->buffer;
	for (unsigned int i = 0; i < nthreads; i++) {
		struct thread_info *ti = &mb->tinfo[i];
		ti->width  = width;
		ti->height = remain >= chunk ? chunk : remain;
		ti->x0     = tx0;
		ti->y0     = ty0;
		ti->x1     = ti->x0 + ti->width * xdelta;
		ti->y1     = ti->y0 + ti->height * ydelta;
		
		ti->buffer = buffer;


		buffer += ti->width * ti->height;
		ty0 += ti->height * ydelta;
		remain -= ti->height;

		/*
		 * Uncomment this fprintf to see which
		 * thread is doing which rectangle
		 *
		 * fprintf(stderr, "i=%d,w=%u,h=%u,x0=%f,y0=%f,x1=%f,y1=%f,buffer=%p\n",
		 * 		i, ti->width, ti->height, ti->x0, ti->y0, ti->x1, ti->y1, ti->buffer);
		 */ 

		pthread_create(&ti->thread, NULL, mbrot2_thread_func, ti);
	}

	for (unsigned int i = 0; i < nthreads; i++) {
		struct thread_info *ti = &mb->tinfo[i];
		pthread_join(ti->thread, NULL);
	}
	return 0;
}

struct pymbrot {
	PyObject_HEAD
	struct mbrot mb;
};

PyObject* pymbrot_exception;

/* mbrot.PrimeStream.__init__() */
static int pymbrot_init(PyObject *self, PyObject *args, PyObject *kwds) {
	struct pymbrot *pymb = (struct pymbrot*) self;
	unsigned int width, height;
	double x0, y0, x1, y1;
	int nthreads = 0;

	int ret;

	static char *argnames[] = {"width", "height", "x0", "y0", "x1", "y1", "nthreads", NULL};

	ret = PyArg_ParseTupleAndKeywords(args, kwds, "IIdddd|I", argnames,
					  &width, &height, &x0, &y0, &x1, &y1, &nthreads);
	if (!ret) {
		/* Exception has already been raised */
		return -1;
	}

	ret = mbrot_init(&pymb->mb, width, height, x0, y0, x1, y1, nthreads);
	if (ret) {
		PyErr_Format(pymbrot_exception, "Failed to initialize PrimeStream: %d", ret);
		return -1;
	}
	return 0;
}

/* To be deallocated */
static void pymbrot_dealloc(PyObject *self) {
	struct pymbrot *pymb = (struct pymbrot*) self;
	/*
	 * fprintf(stderr, "freeing mb.buffer = %p\n", pymb->mb.buffer);
	 */
	free(pymb->mb.buffer);
	/*
	 * fprintf(stderr, "freeing mb.tinfo = %p\n", pymb->mb.tinfo);
	 */
	free(pymb->mb.tinfo);
}

/* mbrot.MandlebrotSet.__repr__() */
static PyObject* pymbrot_str(PyObject *self) {
	return Py_BuildValue("");
}

static PyObject* pymbrotset_get_buffer(PyObject *self) {
	struct pymbrot *pymb = (struct pymbrot*) self;
	size_t sz;
	char *buff;

	buff = mbrot_get_buffer(&pymb->mb, &sz);

	return Py_BuildValue("y#", buff, sz);
}



static PyMethodDef pymbrot_methods[] = {
	{
		"get_buffer",
		(PyCFunction)pymbrotset_get_buffer,
		METH_NOARGS,
		"Returns mandlebotset image buffer"
	},
	{NULL}
};

static PyTypeObject t_mbrot = {
	PyVarObject_HEAD_INIT(NULL, 0)		/* tp_head */
	.tp_name = "mbrot2.MandlebrotSet",
	.tp_basicsize = sizeof(struct pymbrot),
	.tp_dealloc = pymbrot_dealloc,
	.tp_repr = pymbrot_str,
	.tp_flags = Py_TPFLAGS_DEFAULT,
	.tp_doc = "Prime Stream Generator",
	.tp_methods = pymbrot_methods,
	.tp_init = pymbrot_init,
	.tp_alloc = PyType_GenericAlloc,
	.tp_new = PyType_GenericNew,
	.tp_free = PyObject_Del,
};

static char pymbrot_module_docs[] = "Module to generate MandlebrotSet";

static struct PyModuleDef mbrot_module = {
	PyModuleDef_HEAD_INIT,		/* m_base     */
	"mbrot2",			/* m_name     */
	pymbrot_module_docs,		/* m_doc      */
	-1,				/* m_size     */
	NULL,				/* m_methods  */
	NULL,				/* m_slots    */
	0,				/* m_traverse */
	0,				/* m_clear    */
	0,				/* m_free     */
};

PyMODINIT_FUNC
PyInit_mbrot2(void)
{
	PyObject *mbrot = PyModule_Create(&mbrot_module);

	PyType_Ready(&t_mbrot);
	PyObject_SetAttrString(mbrot, "MandlebrotSet", (PyObject*)&t_mbrot);

	pymbrot_exception = PyErr_NewException("mbrot2.MandlebrotSetException", NULL, NULL);
	PyModule_AddObject(mbrot, "MandlebrotSetException", pymbrot_exception);

	return mbrot;
}
