#include <iomanip>
#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>

void print_double_hex(std::ostream& out, double* d, size_t size) 
{
        // number_of_octets correctly calculated because I am on a 
        // POSIX system where CHAR_BIT== 8
	size_t number_of_doubles = size;
	size_t number_of_octets = 8 * number_of_doubles;


	// Make this nicer
	out << "{";
	for ( size_t i = 0; i < number_of_octets - 1; ++i )
	{
		out 
			<< std::hex
			<< std::setfill('0')
			<< "0x"
			<< std::setw(2)
			<< static_cast<int>(reinterpret_cast<uint8_t*>(d)[i])
			<< ", ";
	}
	out 
		<< std::hex << "0x" << static_cast<int>(reinterpret_cast<uint8_t*>(d)[number_of_octets - 1])
		<< "}";
}

void print_double_bin(std::ostream& out, double* d, size_t size)
{
	size_t number_of_doubles = size;
	size_t number_of_octets = 8 * number_of_doubles;

	for ( size_t i = 0; i < number_of_octets; ++i )
	{
		out << reinterpret_cast<uint8_t*>(d)[i];
	}
}

void serialize(std::ostream& out, double* d, size_t size)
{
	print_double_bin(out, d, size);
}

std::vector<double> deserialize(std::istream& in)
{
	return {};
}

void print_double_array()
{
}

int check_double(int argc, char* argv[])
{
	// If we are using POSIX, CHAR_BIT == 8
	//
	// Using uint8_t is guaranteed to be possible 
	
	uint8_t data[] {0x2a, 0x8b, 0xc2, 0x2e, 0xfe, 0x60, 0xea, 0x40};
	double d {*reinterpret_cast<double*>(&data)};

	// size_t number_of_octets = sizeof(data) / ;

	std::cout << std::setprecision(20) << d << "\n";
		
	return 0;
}

std::unique_ptr<double> uint8ArrayToDoubleArray(std::unique_ptr<uint8_t> array)
{
	std::unique_ptr<double> new_array {
		reinterpret_cast<double*>(array.release()) 
	};

	return new_array;
}

std::unique_ptr<uint8_t> toHeap(uint8_t array[], size_t size) {
	uint8_t* _array = (uint8_t*) malloc( size );
	memcpy(_array, &array[0], size);

	return std::unique_ptr<uint8_t> { _array };
}

std::unique_ptr<double> deserializeFromFile(const std::string& filename, size_t& size) {
	FILE* fp = fopen(filename.c_str(), "rb");

	fseek( fp , 0L , SEEK_END);
	size_t file_size = ftell( fp );
	rewind( fp );

	uint8_t* _array = (uint8_t*) malloc(file_size);
	std::unique_ptr<uint8_t> array(_array);

	fread( _array, file_size, sizeof(_array[0]), fp );
	fclose(fp);

	size = file_size / 8;
	return uint8ArrayToDoubleArray(std::move(array));
}

int serialize(int argc, char* argv[])
{
	double d[] {
		54023.943208,
		44564564.4464343575,
		0.46842135452,
		4654121.7756543
	};

	uint8_t _array[] {
		0x2a, 0x8b, 0xc2, 0x2e, 0xfe, 0x60, 0xea, 0x40, 0x2d, 0x4c, 
		0x92, 0xa3, 0x02, 0x40, 0x85, 0x41, 0xbc, 0x54, 0x9a, 0x8f, 
		0x9d, 0xfa, 0xdd, 0x3f, 0xef, 0x51, 0xa4, 0x71, 0x0a, 0xc1, 
		0x51, 0x41
	};

	constexpr char filename[] {"build/double-array-serialization"};
	size_t double_array_size = sizeof(d) / sizeof(d[0]);
	std::unique_ptr<uint8_t> array = toHeap(_array, sizeof(_array));

	// TODO: Ensure that doubles are IEEE 754 binary64 format
	
	std::cout << "Here is the original double array: \n";
	for ( size_t i = 0; i < double_array_size; ++i )
	{
		std::cout << d[i] << "\n";
	}
	std::cout << "\n\n";

	std::cout << "Here is the double array as octets: ";
	print_double_hex(std::cout, &d[0], double_array_size);
	std::cout << "\n\n";

	std::cout << "Writing as binary to file " << filename << "\n";
	{
		std::ofstream outputFile(filename);
		print_double_bin(outputFile, &d[0], double_array_size);
	}

	std::cout << "Reading from literal array\n";
	std::unique_ptr<double> converted = uint8ArrayToDoubleArray(std::move(array));
	for ( size_t i = 0; i < sizeof(_array) / 8; ++i )
	{
		std::cout << converted.get()[i] << "\n";
	}
	std::cout << "\n\n";

	std::cout << "Reading from file " << filename << "\n";
	size_t double_size;
	std::unique_ptr<double> doubles = deserializeFromFile(filename, double_size);
	for ( size_t i = 0; i < double_size; ++i )
	{
		std::cout << doubles.get()[i] << "\n";
	}
	std::cout << "\n\n";

	return 0; 
}

// int main (int argc, char* argv[])
// {
// 	serialize(argc, argv);
// }
