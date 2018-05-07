from distutils.core import setup, Extension

module1 = Extension('PNG_read',
                    sources = ['png.c'],
                    libraries=['png'],
                    extra_compile_args = ["-std=c99"])

setup (name = 'PNG read',
       ext_modules = [module1])
