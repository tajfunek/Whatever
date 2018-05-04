#include "png2.h"

PyObject* read_png(PyObject *self, PyObject *args) {
  //clock_t start = clock()

  // Parsing arguments
  const char* filename;
  if(!PyArgs_ParseTuple(args, "s", filename)) {
    printf("Error while parsing args");
    return NULL;
  }

  // Reading file
  FILE* fp = fopen(filename, "rb");
  if (!fp) {
    return 9;
  }
  // Initializing basic varibles
  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info = png_create_info_struct(png);
  setjmp(png_jmpbuf(png));

  // Connetcing file to PNG_Structp
  png_init_io(png, fp);
  png_read_info(png, info);

  // Closing file
  fclose(fp);

  // Reading basic informations about image
  int width = png_get_image_width(png, info);
  int height = png_get_image_height(png, info);
  png_byte color_type = png_get_color_type(png, info);
  png_byte bit_depth = png_get_bit_depth(png, info);

  // Chceking image
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

  // Reading image
  png_bytep *row_pointers;
  row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
  for(int y = 0; y < height; y++) {
    row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
  }

  png_read_image(png, row_pointers);

  int length = width * height * 4;

  // Converting 2D array row_pointers to 1D pixels for easier handling in python
  int i, j;
  unsigned char* pixels = malloc(length * sizeof(unsigned char));
  for(i = 0; i < height; i++) {
    for(j = 0; j < width; j++) {
      pixels[i*height + j] = row_pointers[i][j]
    }
  }

  // Creating Python List and filling it with metadata
  PyObject* list = PyList_New((Py_ssize_t)length);
  for(i = 0; i < length; i++) {
    if(PyList_SetItem(list, (Py_ssize_t)i, pixels[i]) == -1) {
      printf("Error while filling up list");
      return NULL;
    }

    return list;
  }

  return;
}
