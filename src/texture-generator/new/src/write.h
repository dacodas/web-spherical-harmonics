#include <string>

#include <experimental/filesystem>

namespace std {
	namespace filesystem = experimental::filesystem;
}

typedef struct Directories {
	std::string png_directory;
	std::string double_directory;
} Directories;

namespace Write {
	constexpr unsigned int CreateDirectories = 0x01;
	constexpr unsigned int WriteDouble = 0x02;
	constexpr unsigned int WritePNG = 0x04;
	constexpr unsigned int FillPNGHorizontally = 0x08;
	constexpr unsigned int FillPNGVertically = 0x10;

	using Mode = unsigned int;
}

void write(const std::filesystem::path& relative_path,
	const std::string& filename,
	double* buffer,
	size_t row_size,
	size_t column_size,
	Write::Mode mode);

Directories determineDirectoryNames(const std::filesystem::path& relative_path);
