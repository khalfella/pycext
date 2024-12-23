from setuptools import setup, Extension
setup(name = 'salute', ext_modules=[Extension('salute', sources=['salute.c'])])
