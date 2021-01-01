from distutils.core import setup, Extension
from Cython.Build import cythonize
import distutils.sysconfig
from distutils.sysconfig import get_config_vars as default_get_config_vars

CFLAGS = ["-Wall", "-Wextra", "-pedantic", "-std=c++17", "-O3", "-march=native"]

# default_arguments = default_get_config_vars()
#
#
#
# def cc_flag_interceptor(*args):
#     res = default_get_config_vars(*args)
#
#     if isinstance(res, list):
#         res[-1] += (" ".join(CFLAGS))
#         res = [s.replace("-O2", "-O3").replace("-O1", "-O3") for s in res]
#
#     elif isinstance(res, dict):
#         res = {k: v.replace("-O2", "-O3") for k, v in res.items() if isinstance(v, str)}
#
#     return res
#
#
# distutils.sysconfig.get_config_vars = cc_flag_interceptor


setup(
    ext_modules=cythonize(
        Extension(
            "hpyc",
            sources=["hpyc.pyx"],
            extra_compile_args=CFLAGS,
            extra_link_args=CFLAGS,
            language="c++",
        ),
        compiler_directives={
            "language_level": "3", 
            "boundscheck": False, 
            "wraparound": False, 
            "initializedcheck": False
        }
    )
)
