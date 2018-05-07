/*

 * A simple libpng example program

 * http://zarb.org/~gc/html/libpng.html

 *

 * Modified by Yoshimasa Niwa to make it much simpler

 * and support all defined color_type.

 *

 * To build, use the next instruction on OS X.

 * $ brew install libpng

 * $ clang -lz -lpng15 libpng_test.c

 *

 * Copyright 2002-2010 Guillaume Cottenceau.

 *

 * This software may be freely redistributed under the terms

 * of the X11 license.

 *

 */

#include "png2.h"


PyObject* read_PNG(PyObject* self, PyObject* args) {
  const char* filename;
  if(!PyArg_ParseTuple(args, "s", &filename)) {
    abort();
  }

  int width, height;

  png_byte color_type;

  png_byte bit_depth;

  png_bytep *row_pointers;



  FILE *fp = fopen(filename, "rb");



  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if(!png) abort();



  png_infop info = png_create_info_struct(png);

  if(!info) abort();



  if(setjmp(png_jmpbuf(png))) abort();



  png_init_io(png, fp);



  png_read_info(png, info);



  width      = png_get_image_width(png, info);

  height     = png_get_image_height(png, info);

  color_type = png_get_color_type(png, info);

  bit_depth  = png_get_bit_depth(png, info);



  // Read any color_type into 8bit depth, RGBA format.

  // See http://www.libpng.org/pub/png/libpng-manual.txt



  if(bit_depth == 16)

    png_set_strip_16(png);



  if(color_type == PNG_COLOR_TYPE_PALETTE)

    png_set_palette_to_rgb(png);



  // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.

  if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)

    png_set_expand_gray_1_2_4_to_8(png);



  if(png_get_valid(png, info, PNG_INFO_tRNS))

    png_set_tRNS_to_alpha(png);



  // These color_type don't have an alpha channel then fill it with 0xff.

  if(color_type == PNG_COLOR_TYPE_RGB ||

     color_type == PNG_COLOR_TYPE_GRAY ||

     color_type == PNG_COLOR_TYPE_PALETTE)

    png_set_filler(png, 0xFF, PNG_FILLER_AFTER);



  if(color_type == PNG_COLOR_TYPE_GRAY ||

     color_type == PNG_COLOR_TYPE_GRAY_ALPHA)

    png_set_gray_to_rgb(png);



  png_read_update_info(png, info);



  row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);

  for(int y = 0; y < height; y++) {

    row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));

  }



  png_read_image(png, row_pointers);

  int length = width * height * 4;

  unsigned char* pixels = malloc(length * sizeof(unsigned char));

  for(int i = 0; i < height; i++) {
    //unsigned char* row = row_pointers[i];
    for(int j = 0; j < width; j++) {
      for(int k = 0; k < 4; k++) { 
        pixels[i*height + j + k] = row_pointers[i][j+k];
      }
   }
  }

  PyObject* list = PyList_New((Py_ssize_t)length);
  for(int i = 0; i < length; i++) {
    PyObject* pixel;
    if((pixel = PyLong_FromSize_t((size_t)pixels[i])) == NULL) {
      abort();
    }
    if(PyList_SetItem(list, (Py_ssize_t)i, pixel) == -1) {
      abort();
    }
  }


  fclose(fp);

  return list;

}


/*
void process_png_file() {

  for(int y = 0; y < height; y++) {

    png_bytep row = row_pointers[y];

    for(int x = 0; x < width; x++) {

      png_bytep px = &(row[x * 4]);

      // Do something awesome for each pixel here...

      printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);

    }

  }

}*/


/*
int main(int argc, char *argv[]) {

  if(argc != 2) abort();
  //printf("DDDDDDD");


  read_png_file(argv[1]);
  //printf("SSSSSSSSS");

  process_png_file();
  //printf("AAAAAaAA");

  //write_png_file(argv[2]);



  return 0;

}*/

// Mapping between python and c function names.
static PyMethodDef readModule_methods[] = {
    {"read", read_PNG, METH_VARARGS, "Reads PNG image ASAP"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef pngmodule = {
    PyModuleDef_HEAD_INIT,
    "PNG_read",
    NULL,
    -1,
    readModule_methods
};

// Module initialisation routine.
PyMODINIT_FUNC
PyInit_PNG_read(void) {
    // Init module.
    return PyModule_Create(&pngmodule);

}

