#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>

#include <parallel/algorithm>
#include <gsl/gsl_randist.h>

#include "StrideIterator.h"
#include "rotation.h"
#include "write.h"
#include "serialization.h"
#include "fill-and-convert.h"
#include "png.h"

std::unique_ptr<double> expand(double* vector, size_t size)
{
	double* expanded = (double*) malloc( 3 * size * sizeof(vector[0]) );
	double* edge = (double*) malloc( size );

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

typedef struct Pixel {
	size_t theta;
	size_t phi;
} Pixel;

std::unique_ptr<double> rotateImage(double* image, size_t size, double beta)
{
	constexpr double
		// Theta { M_PI / 4.0d },
		Theta { 0.0d },
		Phi { 0.0d };

	double theta_increment = M_PI / static_cast<double>( size - 1 );
	double phi_increment = 2.0d * M_PI / static_cast<double>( size - 1 );

	std::vector<Pixel> pixels {};
	pixels.reserve(size * size);

	for ( size_t th = 0 ; th < size ; ++th )
		for ( size_t ph = 0 ; ph < size ; ++ph )
			pixels.emplace_back(Pixel({th, ph}));

	double* new_image = (double*) malloc( size * size * sizeof(double));
	std::fill_n(new_image, size * size, 0.0d);

	// __gnu_parallel::for_each(
	std::for_each(
		pixels.begin(), pixels.end(),
		[&] (const Pixel& pixel)
		{
			double theta { theta_increment * pixel.theta };
			double phi { phi_increment * pixel.phi };
			double _rotated_theta { rotatedTheta(theta, phi, Theta, Phi, beta) };
			double _rotated_phi { rotatedPhi(theta, phi, Theta, Phi, beta) };

			double rotated_theta = _rotated_theta;
			double rotated_phi = _rotated_phi < 0.0d ? _rotated_phi + 2.0d * M_PI : _rotated_phi;

			double rtd { std::floor( rotated_theta / theta_increment ) };
			double rpd { std::ceil( rotated_phi / phi_increment ) };
			size_t rotated_theta_index { static_cast<size_t>( rtd ) };
			size_t rotated_phi_index { static_cast<size_t>( rpd ) };

			// std::cout
			// 	<< "(" << theta << ", " << phi << ")"
			//        	<< " -> "
			// 	<< "(" << _rotated_theta << ", " << _rotated_phi << ")\n";

			// std::cout
			// 	<< "(" << theta << ", " << phi << ")"
			//        	<< " -> "
			// 	<< "(" << rotated_theta << ", " << rotated_phi << ")\n";

			// std::cout
			// 	<< "(" << rtd << ", " << rpd << ")\n";

			// std::cout
			// 	<< "(" << pixel.theta << ", " << pixel.phi << ")"
			// 	<< " -> "
			// 	<< "(" << rotated_theta_index << ", " << rotated_phi_index << ")\n";

			// std::cout << "|\n";

			new_image[size * rotated_theta_index + rotated_phi_index] = \
				image[size * pixel.theta + pixel.phi];
		});

	return std::unique_ptr<double> {new_image};
}

// void rotateHarmonic(size_t size)
// {
// 	write("rotate", "", nullptr, 0, 0, Write::CreateDirectories);
//
// 	size_t harmonic_size;
// 	std::unique_ptr<double> harmonic;
// 	{
// 		harmonic = deserializeFromFile("build/harmonics/double/harmonic-005-002.double", harmonic_size);
// 		transpose(harmonic.get(), size);
// 	}
//
// 	constexpr size_t steps {200};
// 	constexpr double increment = 2.0d * M_PI / static_cast<double>( steps );
//
// 	for ( size_t step = 0 ; step < steps; ++step )
// 	{
// 		std::stringstream filename {};
// 		filename << "wavelet-" << std::setw(3) << std::setfill('0') << step ;
//
// 		std::cout << "Writing " << step << "\n";
//
// 		double beta = step * increment;
// 		std::unique_ptr<double> rotate_image {(double*) malloc(size * size * sizeof(double))};
// 		__gnu_parallel::transform(
// 				harmonic.get(), harmonic.get() + size * size,
// 				rotate_image.get(),
// 				[] (double d1) -> double { return d1 * d2; });
// 		write("wavelet", filename.str(), combination_image.get(), size, size, max, Write::WritePNG | Write::WriteDouble);
// 	}
// }

int main()
{
	// uint16_t* combination_image_int = toUint16(combination_image, size * size);
	// write_png("combination.png", combination_image_int, size);
	// {
	// 	std::ofstream file {"combination.double"};
	// 	serialize(file, combination_image, size * size);
	// }

	constexpr size_t size = 100;
	// convolveWaveletAnimation(size);
	size_t harmonic_size;
	std::unique_ptr<double> harmonic = deserializeFromFile("build/harmonics/double/harmonic-005-002.double", harmonic_size);
	// transpose(harmonic.get(), size);

	write("wavelet", "normal", harmonic.get(), 100, 100, Write::WritePNG | Write::WriteDouble);

	for ( size_t step = 0; step < 100; ++step )
	{
		std::cout << "Doing " << step << "\n";
		double beta = 2 * ( M_PI / 100.0d ) * step;
		std::stringstream filename {};
		filename << "rotated-" << std::setfill('0') << std::setw(3) << step;

		std::unique_ptr<double> rotated = rotateImage(harmonic.get(), 100, beta);
		write("wavelet", filename.str(), rotated.get(), 100, 100, Write::WritePNG | Write::WriteDouble);
	}


	// test();

	return 0;
}
