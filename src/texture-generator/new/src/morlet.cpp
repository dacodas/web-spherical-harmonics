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

std::unique_ptr<double> generateWavelet(size_t size)
{
	constexpr double gaussian_boundary = 5.0d;
	constexpr double gaussian_sigma = 0.5d;
	constexpr double cos_boundary = M_PI_2;
	constexpr double cos_frequency = 20;

	double cos_increment = 2 * cos_boundary / ( size - 1 );
	double gaussian_increment = 2 * gaussian_boundary / ( size - 1 );

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

	std::unique_ptr<double> wavelet {(double*) malloc(size * sizeof(double))};

	__gnu_parallel::transform(
			gaussian, gaussian + size,
			cosine,
			wavelet.get(),
			[] (double d1, double d2) -> double { return d1 * d2; });

	free(gaussian);
	free(cosine);
	
	return wavelet;
}

void convolveWaveletAnimation(size_t size)
{
	write("wavelet", "", nullptr, 0, 0, Write::CreateDirectories);

	std::unique_ptr<double> wavelet = generateWavelet(size);
	std::unique_ptr<double> wavelet_image {fill_horizontally(wavelet.get(), size, size)};
	write("wavelet", "wavelet", wavelet_image.get(), size, size, Write::WritePNG | Write::WriteDouble);

	std::unique_ptr<double> expanded_wavelet = expand(wavelet.get(), size);
	std::unique_ptr<double> expanded_wavelet_image {fill_horizontally(expanded_wavelet.get(), 3 * size, size)};
	write("wavelet", "expanded_wavelet", expanded_wavelet_image.get(), 3 * size, size, Write::WritePNG | Write::WriteDouble);

	size_t harmonic_size;
	std::unique_ptr<double> harmonic;
	{
		harmonic = deserializeFromFile("build/harmonics/double/harmonic-005-002.double", harmonic_size);
		transpose(harmonic.get(), size);
	}

	double sigma = 1.0d;
	double gaussian_max = gsl_ran_gaussian_pdf(0.0d, sigma);
	double max = max_abs(harmonic.get(), harmonic_size) * gaussian_max;
	for ( size_t offset = 0 ; offset <= 200 ; ++offset )
	{
		std::stringstream filename {};
		filename << "combined-" << std::setw(3) << std::setfill('0') << offset ;

		std::cout << "Writing " << offset << "\n";

		double x = ( static_cast<double>( offset ) - 100 ) / 20.0d;
		double scale = gsl_ran_gaussian_pdf(x, 1);

		StrideIterator<double> strided(expanded_wavelet_image.get() + offset, size, 3 * size);
		std::unique_ptr<double> combination_image {(double*) malloc(size * size * sizeof(double))};
		__gnu_parallel::transform(
				harmonic.get(), harmonic.get() + size * size,
				strided,
				combination_image.get(),
				[&scale] (double d1, double d2) -> double { return scale * d1 * d2; });
		write("wavelet", filename.str(), combination_image.get(), size, size, max, Write::WritePNG | Write::WriteDouble);
	}
}

int main() 
{
	// uint16_t* combination_image_int = toUint16(combination_image, size * size);
	// write_png("combination.png", combination_image_int, size);
	// {
	// 	std::ofstream file {"combination.double"};
	// 	serialize(file, combination_image, size * size);
	// }
	
	constexpr size_t size = 100;
	convolveWaveletAnimation(size);

	return 0;
}
