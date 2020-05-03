#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>

#include <parallel/algorithm>
#include <gsl/gsl_randist.h>

#include "write.h"
#include "serialization.h"
#include "fill-and-convert.h"
#include "png.h"

template<typename T>
class StrideIterator {
	private: 
		size_t row_position;
		size_t row_size;
		size_t stride_size;
		T* _it;

	public:

		using difference_type = size_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::input_iterator_tag;

		StrideIterator<T>(T* it, size_t row_size, size_t stride_size) 
			: _it {it},
			row_position {0}, 
			row_size {row_size},
			stride_size {stride_size} {}

		T operator*()
		{
			return *_it;
		}

		StrideIterator<T> operator++()
		{
			if ( row_position == row_size - 1 )
			{
				_it -= row_position;
				_it += stride_size;
				row_position = 0;
			}
			else 
			{
				++_it;
				++row_position;
			}
			return *this;
		}

		friend bool operator!=( const StrideIterator<T> lhs, const StrideIterator<T> rhs )
		{
			return lhs._it != rhs._it;
		}

		friend StrideIterator<T> operator+(StrideIterator<T> lhs, int rhs)
		{
			for ( size_t i = 0 ; i < rhs ; ++i )
			{
				++lhs;
			}
			return lhs;
		}
};

std::unique_ptr<double> expand(double* vector, size_t size) 
{
	double* expanded = (double*) malloc( 3 * size * sizeof(vector[0]) );
	double* edge = (double*) malloc( size);

	for ( size_t i = 0; i < size; ++i )
	{
		expanded[i] = vector[0];
		expanded[2 * size + i] = vector[size - 1];
	}

	memcpy(expanded + size, vector, size * sizeof(vector[0]));
	free(edge);

	return std::unique_ptr<double> {expanded};
}

int main() 
{
	constexpr size_t size = 100;
	constexpr double gaussian_boundary = 5.0d;
	constexpr double gaussian_increment = 2 * gaussian_boundary / ( size - 1 );
	constexpr double gaussian_sigma = 0.5d;
	constexpr double cos_boundary = M_PI_2;
	constexpr double cos_increment = 2 * cos_boundary / ( size - 1 );
	constexpr double cos_frequency = 40;

	// std::vector<double> gaussian(size);
	// std::vector<double> cosine(size);
	double* gaussian = (double*) malloc(size * sizeof(gaussian[0]));
	double* cosine = (double*) malloc(size * sizeof(cosine[0]));

	std::generate(
			gaussian, gaussian + size,
			[&] () -> double { 
			static double x = -gaussian_boundary;
			double result = gsl_ran_gaussian_pdf(x, gaussian_sigma); 
			x += gaussian_increment;
			return result;
			});

	std::generate(
			cosine, cosine + size,
			[&] () -> double { 
			static double x = -cos_boundary;
			double result = cos(cos_frequency * x);
			x += cos_increment;
			return result;
			});

	// double* gaussian_image = fill_horizontally(gaussian, size);
	// double* cosine_image = fill_vertically(cosine, size);
	// double* cosine_image = fill_horizontally(cosine, size);
	// __gnu_parallel::transform(
	// 		gaussian_image, gaussian_image + size * size,
	// 		cosine_image,
	// 		new_image,
	// 		[] (double d1, double d2) -> double { return d1 * d2; });

	std::unique_ptr<double> wavelet {(double*) malloc(size * sizeof(double))};
	std::unique_ptr<double> wavelet_image {(double*) malloc(size * size * sizeof(double))};

	__gnu_parallel::transform(
			gaussian, gaussian + size,
			cosine,
			wavelet.get(),
			[] (double d1, double d2) -> double { return d1 * d2; });

	std::unique_ptr<double> expanded = expand(wavelet.get(), size);
	std::unique_ptr<double> expanded_matrix {fill_horizontally(expanded.get(), 3 * size, size)};
	write("wavelet", "", nullptr, 0, 0, Write::CreateDirectories);

	write("wavelet", "expanded", expanded_matrix.get(), 3 * size, size, Write::WritePNG | Write::WriteDouble);

	std::unique_ptr<double> wavelet_matrix {fill_horizontally(wavelet.get(), size, size)};
	write("wavelet", "wavelet", wavelet_matrix.get(), size, size, Write::WritePNG | Write::WriteDouble);

	size_t harmonic_size;
	std::unique_ptr<double> harmonic = deserializeFromFile("build/harmonics/double/harmonic-005-005.double", harmonic_size);
	transpose(harmonic.get(), size);

	double max = max_abs(harmonic.get(), harmonic_size);
	for ( size_t offset = 0 ; offset <= 200 ; ++offset )
	{
		std::stringstream filename {};
		filename << "combined-" << std::setw(3) << std::setfill('0') << offset ;

		std::cout << "Writing " << offset << "\n";

		StrideIterator<double> strided(expanded_matrix.get() + offset, size, 3 * size);
		std::unique_ptr<double> combination_image {(double*) malloc(size * size * sizeof(double))};
		__gnu_parallel::transform(
				harmonic.get(), harmonic.get() + size * size,
				strided,
				combination_image.get(),
				[] (double d1, double d2) -> double { return d1 * d2; });
		write("wavelet", filename.str(), combination_image.get(), size, size, max, Write::WritePNG | Write::WriteDouble);
	}

	// uint16_t* combination_image_int = toUint16(combination_image, size * size);
	// write_png("combination.png", combination_image_int, size);
	// {
	// 	std::ofstream file {"combination.double"};
	// 	serialize(file, combination_image, size * size);
	// }

	return 0;
}
