from setuptools import setup, Extension

# from setuptools.command.build_ext import build_ext
import sys
import os
from pybind11 import get_cmake_dir
from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

__version__ = '0.1.0'

ext_modules = [
    Pybind11Extension(
        'pymvaformat',
        # Sort input source files to ensure bit-for-bit reproducible builds
        # (https://github.com/pybind/python_example/pull/53)
        (['pymvaformat_pybind11.cpp',
          'pymvaformat_pybind11_scene.cpp',
          '..external/zstdstream/zstdstream.cpp']),
        include_dirs=[
            # Path to pybind11 headers
            '..',
            '../..',
            '/usr/include/eigen3'
        ],
        libraries=['z'],
        language='c++',
        extra_compile_args=["-D_USE_GZSTREAM",
                            "-std=c++14"]
    ),
]

setup(
    name='mvaformat',
    version=__version__,
    author='Shaun Song',
    author_email='song.1634@osu.edu',
    url='',
    description='A python reader/writer of .mva compression file.',
    long_description='',
    ext_modules=ext_modules,
    setup_requires=['pybind11>=2.5.0'],
    cmdclass={'build_ext': build_ext},
    zip_safe=False,
)
