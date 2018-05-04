#include "png2.h"

void abort_(const char * s, ...) {
        va_list args;
        va_start(args, s);
        vfprintf(stderr, s, args);
        fprintf(stderr, "\n");
        va_end(args);
        abort();
}

PyObject* read_PNG(PyObject *self, PyObject *args) {
  //clock_t start = clock()
  abort_("Error");
  // Parsing arguments
  const char* filename;
  if(!PyArg_ParseTuple(args, "s", &filename)) {
    abort_("Error while parsing args");
    return NULL;
  }

  printf("Filename: %s \n", filename);

  // Reading file
  FILE* fp = fopen(filename, "rb");
  if (!fp) {
    abort_("Error while reading file");
    return NULL;
  }

  printf("File read\n\n");
  int i = 0;

  unsigned char header[8];
  fread(header, 1, 8, fp);
  if (png_sig_cmp(header, 0, 8)) {
    abort_("[read_png_file] File %s is not recognized as a PNG file", filename);
    
  }
  printf("filetype correct");

  // Initializing basic varibles
  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info = png_create_info_struct(png);
  setjmp(png_jmpbuf(png));

  printf("Image initialized:  %i\n\n", i++);

  // Connetcing file to PNG_Structp
  png_init_io(png, fp);
  png_set_sig_bytes(png, 8);
  png_read_info(png, info);

  printf("Image read\n\n");

  // Closing file
  fclose(fp);

  // Reading basic informations about image
  int width = png_get_image_width(png, info);
  int height = png_get_image_height(png, info);
  png_byte color_type = png_get_color_type(png, info);
  png_byte bit_depth = png_get_bit_depth(png, info);

  // Checking image
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

  printf("Sth has been don\n\n");

  // Reading image
  png_bytep *row_pointers;
  row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
  int y;
  printf("Row pointer init\n\n");
  for(y = 0; y < height; y++) {
    row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
  }
  printf("png_row_bytes\n\n");

  png_read_image(png, row_pointers);

  printf("Row pointer done\n\n");

  int length = width * height * 4;

  // Converting 2D array row_pointers to 1D pixels for easier handling in python
  int j;
  unsigned char* pixels = malloc(length * sizeof(unsigned char));
  for(i = 0; i < height; i++) {
    for(j = 0; j < width; j++) {
      pixels[i*height + j] = row_pointers[i][j];
    }
  }

  // Creating Python List and filling it with metadata
  PyObject* list = PyList_New((Py_ssize_t)length);
  for(i = 0; i < length; i++) {
    PyObject* pixel;
    if((pixel = PyLong_FromSize_t((size_t)pixels[i])) == NULL) {
      abort_("Error while PyLonging");
      return NULL;
    }
    if(PyList_SetItem(list, (Py_ssize_t)i, pixel) == -1) {
      abort_("Error while filling up list");
      return NULL;
    }
  }

  return list;
}


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
