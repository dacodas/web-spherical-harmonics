#include "write.h"

#include <fstream>

#include "serialization.h"
#include "fill-and-convert.h"
#include "png.h"

Directories determineDirectoryNames(const std::filesystem::path& relative_path) {
	std::stringstream
		png_output_folder_stream {},
		double_output_folder_stream {};

	std::string
		png_output_folder,
		double_output_folder;

	png_output_folder_stream << "build/" << relative_path.c_str() << "/png";
	double_output_folder_stream << "build/" << relative_path.c_str() << "/double";

	png_output_folder = std::move(png_output_folder_stream.str());
	double_output_folder = std::move(double_output_folder_stream.str());

	return Directories {png_output_folder, double_output_folder};
}

void write(const std::filesystem::path& relative_path,
	const std::string& filename,
	double* buffer,
	size_t row_size,
	size_t column_size,
	Write::Mode mode)
{
	Directories directories {determineDirectoryNames(relative_path)};

	if ( mode & Write::CreateDirectories )
	{
		std::filesystem::create_directories(directories.png_directory);
		std::filesystem::create_directories(directories.double_directory);
	}

	if ( mode & Write::WriteDouble )
	{
		std::stringstream double_filename_stream {};
		std::string double_filename {};
		double_filename_stream << directories.double_directory.c_str() << "/" << filename << ".double";
		double_filename = std::move(double_filename_stream.str());
		std::ofstream double_output {double_filename};
		serialize(double_output, buffer, row_size * column_size);
	}

	if ( mode & Write::WritePNG )
	{
		std::stringstream png_filename_stream {};
		std::string png_filename {};
		png_filename_stream << directories.png_directory.c_str() << "/" << filename << ".png";
		png_filename = std::move(png_filename_stream.str());
		uint16_t* uint16_image = toUint16(buffer, row_size * column_size);

		write_png(png_filename, uint16_image, row_size, column_size);
	}
}
