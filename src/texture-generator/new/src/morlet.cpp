#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>

#include <parallel/algorithm>
#include <gsl/gsl_randist.h>

#include "serialization.h"
#include "fill-and-convert.h"
#include "png.h"

template<typename T>
class StrideIterator {
	public:

		using difference_type = size_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::input_iterator_tag;

		T* _it;

		T operator*()
		{
			return *_it;
		}

		StrideIterator<T> operator++()
		{
			++_it;
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

		// T operator++()
		// {
		// 	T tmp(_it);
		// 	operator++();
		// 	return tmp;
		// }
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

	double test[] {1., 2., 3., 4., 5.};

	StrideIterator<double> testy {};
	testy._it = &test[0];

	for ( size_t i = 0; i < 5; ++i )
	{
		std::cout << *testy;
		++testy;
	}

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
	std::unique_ptr<uint16_t> expanded_int {toUint16(expanded_matrix.get(), 3 * size * size)};
	write_png("wavelet-expanded.png", expanded_int.get(), 3 * size, size);

	std::unique_ptr<double> wavelet_matrix {fill_horizontally(wavelet.get(), size, size)};
	std::unique_ptr<uint16_t> wavelet_int {toUint16(wavelet_matrix.get(), size * size)};
	write_png("wavelet.png", wavelet_int.get(), size, size);

	size_t harmonic_size;
	std::unique_ptr<double> harmonic = deserializeFromFile("build/actual-harmonic-005-005.double", harmonic_size);
	transpose(harmonic.get(), size);

	std::unique_ptr<double> combination_image {(double*) malloc(size * size * sizeof(double))};
	__gnu_parallel::transform(
			expanded_matrix.get(), expanded_matrix.get() + size * size,
			harmonic.get(),
			combination_image.get(),
			[] (double d1, double d2) -> double { return d1 * d2; });

	// uint16_t* combination_image_int = toUint16(combination_image, size * size);
	// write_png("combination.png", combination_image_int, size);
	// {
	// 	std::ofstream file {"combination.double"};
	// 	serialize(file, combination_image, size * size);
	// }

	return 0;
}
