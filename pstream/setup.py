from setuptools import setup, Extension
setup(name = 'pstream', ext_modules=[Extension('pstream', sources=['pstream.c'])])
