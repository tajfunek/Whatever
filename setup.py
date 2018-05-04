from distutils.core import setup, Extension

module1 = Extension('demo',
                    sources = ['png.c'],
                    libraries=['png'])

setup (name = 'PNG read',
       ext_modules = [module1])
