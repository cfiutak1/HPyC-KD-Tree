from distutils.core import setup, Extension
from Cython.Build import cythonize
# from distutils.sysconfig import get_config_vars as default_get_config_vars
# default_arguments = default_get_config_vars()

# print(default_get_config_vars('CC', 'CXX', 'CFLAGS',
#                 'CCSHARED', 'LDSHARED', 'SHLIB_SUFFIX', 'AR', 'ARFLAGS',
#                 'CONFIGURE_CPPFLAGS', 'CONFIGURE_CFLAGS', 'CONFIGURE_LDFLAGS'))
# def foo(*args):
#     res = default_get_config_vars(*args)
#
#     # print(type(res))
#     # assert isinstance(res, list)
#
#     # print(res)
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
# import distutils.sysconfig as dsc
# dsc.get_config_vars = foo


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
