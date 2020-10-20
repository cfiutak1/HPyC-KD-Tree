from distutils.core import setup, Extension
from Cython.Build import cythonize
# from Cython.Compiler.Main import default_options
# default_options['emit_linenums'] = True

CFLAGS = ["-Wall", "-Wextra", "-pedantic", "-std=c++17", "-flto", "-pthread", "-fopenmp", "-O3", "-march=native", "-fno-wrapv"]

setup(
    ext_modules=cythonize(
        Extension(
            "hpyc",
            sources=["hpyc.pyx"],
            extra_compile_args=CFLAGS,
            language="c++",
        ),
        compiler_directives={'language_level': "3", 'boundscheck': False, "wraparound": False, "initializedcheck": False}
    )
)