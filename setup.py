from distutils.core import setup, Extension
from Cython.Build import cythonize

setup(
    ext_modules=cythonize(
        Extension(
            "neighbor", # the extension name
            sources=["neighbor.pyx"], # the Cython source and additional C++ source files
            language="c++" # generate and compile C++ code
        ),
        compiler_directives={'language_level' : "3"}
    )
)