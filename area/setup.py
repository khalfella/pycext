from setuptools import setup, Extension
setup(name = 'area', ext_modules=[Extension('area', sources=['area.c'])])
