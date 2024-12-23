from setuptools import setup, Extension
setup(name = 'greet', ext_modules=[Extension('greet', sources=['greet.c'])])
