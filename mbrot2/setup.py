from setuptools import setup, Extension
setup(name = 'mbrot2', ext_modules=[Extension('mbrot2', sources=['mbrot2.c'])])
