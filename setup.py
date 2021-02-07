# from distutils.core import setup, Extension
# from Cython.Build import cythonize
# import distutils.sysconfig
# from distutils.sysconfig import get_config_vars as default_get_config_vars
#


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


# setup(
#     ext_modules=cythonize(
#         Extension(
#             "hpyc",
#             sources=["hpyc.pyx"],
#             extra_compile_args=CFLAGS,
#             extra_link_args=CFLAGS,
#             language="c++",
#         ),
#         compiler_directives={
#             "language_level": "3",
#             "boundscheck": False,
#             "wraparound": False,
#             "initializedcheck": False
#         }
#     )
# )

import numpy
from numpy.distutils.misc_util import Configuration, get_info
from numpy.distutils.core import setup

CFLAGS = ["-Wall", "-Wextra", "-pedantic", "-std=c++17", "-O3", "-march=native"]

def configuration(parent_package="", top_path=None):
    config = Configuration("", parent_package, top_path)
    config.add_extension(
        "hpyc",
        ["hpyc.cpp"],
        extra_info=get_info("npymath"),
        extra_compile_args=CFLAGS
    )

    return config


setup(configuration=configuration)