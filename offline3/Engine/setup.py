from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'Chain_reaction',
        ['bindings.cpp'],
        include_dirs=[pybind11.get_include()],
        language='c++'
    ),
]

setup(
    name='Chain_reaction',
    ext_modules=ext_modules,
    zip_safe=False,
)