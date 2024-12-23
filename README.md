## Extending Python in C

- The basic idea is to write C code that is usable in python.
- C code is compiled into python modules that python can 'import' and use
  just like native python modules.

- Why extending python? Why C?
  - Implement existing functionality, hopefully faster in lower-level language.
  - Letting python code access existing libraries in C, or other lower-level language.
  - Make use of C native threads (comp to free-threaded python-3.13?)



### Hello world example:

#### Hello

```
$ cd hello
$ python setup.py install
$ python3
>>> import hello
>>> hello.__doc__
>>> exit()
```

hello is minimal example of module that does nothing. It is good to show how a module
is defined. The only thing that can be read here is the doc string. Other than that
the module is completely useless as it contains now methods or other member variables.


#### Greet

```
$ cd greet
$ python setup.py install
$ python3
>>> import greet
>>> greet.greet()
>>> exit()
```

greet adds greet() function at the module level. The function takes not args as in
METH_NOARGS. The only self argument points to the module in this case. The return
value is the interesting part here. Py_BuildValue() builds a string "s" from C string.


#### Salute

```
$ cd salute
$ python setup.py install
$ python3
>>> import salute
>>> salute.salute("Mohamed")
>>> salute.salute("Mohamed", "Khalfella")
>>> exit()
```

salute adds the ability to pass arguments from python to C. PyArg_ParseTuple() helps
in parsing function arguments and converts them to corresponding C types. Coupled
with Py_BuildValue() it is possible to convert C types back to python types as return
value. It also gives example of the use of optional arguments.

#### Area

```
$ cd area
$ python setup.py install
$ python3
>>> import area
>>> area.get_area(2)
>>> area.get_area(2, 2)
>>> area.get_area(width=4, height=3)
>>> area.get_area(0, units="km")
>>> exit()
```

area modules demonstrates the use of keyword arguments and setting default values. get_area() throws an exception if area is 0, serving as example how of throwing exceptions from C code.

#### PrimeStream

```
$ cd pstream
$ python setup.py install
$ python3
>>> import pstream
>>> ps = pstream.PrimeStream()
>>> ps.get(), ps.get(), ps.get()
(2, 3, 5)
>>> ps.get(), ps.get(), ps.get()
(7, 11, 13)
>>> ps = pstream.PrimeStream(123456)
>>> ps.get(), ps.get(), ps.get()
(123457, 123479, 123491)
>>> ps.get(), ps.get(), ps.get()
(123493, 123499, 123503)

>>> # Try large number and see it take time
>>> ps = pstream.PrimeStream(800000000)
>>> ps.get(), ps.get(), ps.get()
(800000011, 800000027, 800000029)
>>> ps.get(), ps.get(), ps.get()
(800000041, 800000063, 800000083)
>>>
```

pstream is a module that shows how to create a new type in a C extension. `PrimeStream` is a class whose object return sequence of prime numbers. The constructor takes optional starting prime number.


### Mandelbrot

```
$ cd mbrot1
$ python setup.py install
$ python3
>>> import mbrot1
>>> from PIL import Image
>>> mb = mbrot1.MandlebrotSet(100, 100, -3, -3, 3, 3)
>>> b = mb.get_buffer()
>>> img = Image.frombuffer('L', (100,100), b, "raw")
>>> img.save("mandelbrot.bmp", "BMP")

>>> mb = mbrot1.MandlebrotSet(10000, 10000, -3, -3, 3, 3)
>>> b = mb.get_buffer()
>>> img = Image.frombuffer('L', (10000,10000), b, "raw")
>>> img.save("mandelbrot.bmp", "BMP")
```

Draw greyscale image of Mandelbrot set. MandlebrotSet constructor takes width and height of the image followed by x0, y0, x1, y1 for coordinates of rectangle to draw.


### Mandelbrot2

```
$ cd mbrot2
$ python setup.py install
$ python3
>>> import mbrot2
>>> from PIL import Image
>>> mb = mbrot2.MandlebrotSet(100, 100, -3, -3, 3, 3, 1)
>>> b = mb.get_buffer()
>>> img = Image.frombuffer('L', (100,100), b, "raw")
>>> img.save("mandelbrot.bmp", "BMP")

>>> mb = mbrot2.MandlebrotSet(10000, 10000, -3, -3, 3, 3, 40)
>>> b = mb.get_buffer()
>>> img = Image.frombuffer('L', (10000,10000), b, "raw")
>>> img.save("mandelbrot.bmp", "BMP")
```

Similar to mbrot1 modules, but it adds the option to use multiple threads to build the image buffer.


See https://docs.python.org/3/c-api/index.html

