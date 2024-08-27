#include <png.h>
#include <stdlib.h>
#include "image.h"

// Function to generate a PNG file using libpng
void generate_png(const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if(!fp) return;

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) return;

    png_infop info = png_create_info_struct(png);
    if (!info) return;

    if (setjmp(png_jmpbuf(png))) return;

    png_init_io(png, fp);

    // Output is 8-bit depth, RGBA format
    png_set_IHDR(
        png,
        info,
        256, 256,
        8,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );
    png_write_info(png, info);
 
    png_bytep row = (png_bytep) malloc(4 * 256 * sizeof(png_byte));

    for(int y = 0; y < 256; y++) {
        for(int x = 0; x < 256; x++) {
            row[x*4] = 255;     // Red
            row[x*4 + 1] = 255; // Green
            row[x*4 + 2] = 255; // Blue
            row[x*4 + 3] = 255; // Alpha
        }
        png_write_row(png, row);
    }

    png_write_end(png, NULL);
    fclose(fp);

    if (png && info)
        png_destroy_write_struct(&png, &info);
    if (row)
        free(row);
}
