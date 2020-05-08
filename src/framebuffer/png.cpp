#include <png.h>
#include <cstdint>

void write_png(size_t row_size, size_t row_number, uint8_t* image, const char* filename)
{
	FILE* fp;
	png_structp png;
	png_infop info;

	fp = fopen(filename, "wb");

	png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png) return;

	info = png_create_info_struct(png);
	if (!info) return;

	png_init_io(png, fp);
	png_set_IHDR(
			png,
			info,
			row_size, row_number,
			8,
			PNG_COLOR_TYPE_RGBA,
			PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT,
			PNG_FILTER_TYPE_DEFAULT
		    );
	png_write_info(png, info);
	png_set_swap(png);


	for ( size_t i = 0; i < row_number; ++i )
	{
		uint8_t* row_start = image + 4 * row_size * i;
		png_write_row(png, (png_const_bytep) row_start);
	}

	png_write_end(png, NULL);
	png_destroy_write_struct(&png, &info);
	fclose(fp);
}
