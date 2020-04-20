#include "png.h"

void png_setup(png_structp* png, png_infop* info, FILE* fp, size_t image_size)
{
	*png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!*png) exit(1);

	*info = png_create_info_struct(*png);
	if (!*info) exit(1);

	png_init_io(*png, fp);
	png_set_IHDR(
			*png, 
			*info,
			image_size, image_size,
			16,
			PNG_COLOR_TYPE_GRAY,
			PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT,
			PNG_FILTER_TYPE_DEFAULT
		    );
	png_write_info(*png, *info);
	png_set_swap(*png);
}

void png_teardown(png_structp* png, png_infop* info)
{
	png_structp _png = *png;
	png_infop _info = *info;

	png_write_end(*png, NULL);

	png_destroy_write_struct(png, info);
}


void write_png(
	const std::string& filename, 
	uint16_t* image,
	size_t image_size)
{
	png_structp png;
	png_infop info;
	FILE* fp;

	fp = fopen(filename.c_str(), "wb");
	png_setup(&png, &info, fp, image_size);

	for ( size_t i = 0; i < image_size; ++i )
	{
		uint16_t* row_start = image + image_size * i;
		png_write_row(png, (png_const_bytep) row_start);
	}

	png_teardown(&png, &info);
	fclose(fp);
}

void write_png(
	const std::string& filename, 
	Eigen::Matrix<uint16_t, Eigen::Dynamic, Eigen::Dynamic>& image,
	size_t image_size
	)
{
	png_structp png;
	png_infop info;
	FILE* fp;

	fp = fopen(filename.c_str(), "wb");
	png_setup(&png, &info, fp, image_size);

	short unsigned int* data = image.data();
	for ( size_t i = 0; i < image_size; ++i )
	{
		short unsigned int* row_start = image.data() + image_size * i;
		png_write_row(png, (png_const_bytep) row_start);
	}

	png_teardown(&png, &info);
}
