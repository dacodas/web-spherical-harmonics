#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <vector>
#include <array>
#include <fstream>

#include <parallel/algorithm>

#include <gsl/gsl_sf_legendre.h>

#include "write.h"
#include "fill-and-convert.h"

typedef struct Cosines {
	double* cos_theta;
	double** cos_m_phi;
	size_t row_size;
	size_t max_l;
} Cosines;

typedef struct LegendrePolynomials {
	double* plm;
	size_t length;
	size_t number;
	size_t max_l;
} LegendrePolynomials;

void writeCosines(Cosines* cosines) {
	size_t size = cosines->row_size;

	std::unique_ptr<double> cos_theta_image {fill_horizontally(cosines->cos_theta, size)};
	write("cos", "cos-theta", cos_theta_image.get(), size, size, Write::CreateDirectories | Write::WritePNG | Write::WriteDouble );

	write("cos/cos-m-phi", "", nullptr, 0, 0, Write::CreateDirectories);
	std::stringstream output_filename_stream;

	for ( size_t m = 0; m < cosines->max_l; ++m )
	{
		std::cout << "Writing cosine " << m << "\n";

		output_filename_stream = {};
		output_filename_stream << std::setw(3) << std::setfill('0') << m;

		// This just comes and then gets immediately unallocated. 
		// Should allow for fill_{horizontally,vertically} to write to 
		// an already allocated region
		std::unique_ptr<double> cos_m_phi_image {fill_vertically(cosines->cos_m_phi[m], size)};
		write("cos/cos-m-phi", output_filename_stream.str(), cos_m_phi_image.get(), size, size, Write::WritePNG | Write::WriteDouble);
	}
}

void initializeCosines(Cosines* cosines, size_t size, size_t max_l)
{
	double* cos_theta = (double*) malloc( size * sizeof(cos_theta[0]) );
	double** cos_m_phi = (double**) malloc( max_l * sizeof(cos_m_phi[0]) );

	for ( size_t i = 0; i < max_l; ++i )
	{
		cos_m_phi[i] = (double*) malloc( size * sizeof(cos_m_phi[i][0]) );
	}

	double theta_increment = M_PI / ( size - 1 );
	double phi_increment = 2 * M_PI / ( size - 1 );
	for ( size_t i = 0; i < size; ++i )
	{
		cos_theta[i] = cos( i * theta_increment );

		for ( size_t m = 0; m < max_l; ++m )
		{
			cos_m_phi[m][i] = cos( m * i * phi_increment );
		}
	}

	*cosines = {cos_theta, cos_m_phi, size, max_l};
}

void initializePolynomials(
		LegendrePolynomials* polynomials,
		Cosines* cosines,
		size_t max_l,
		size_t row_length
		)
{
	size_t result_size = gsl_sf_legendre_array_n(max_l);
	double* plm = (double*) malloc( result_size * row_length * sizeof(plm[0]) );

	// calculate first and last
	double* result_first = (double*) malloc( result_size * sizeof(result_first[0]) );
	double* result_last = (double*) malloc( result_size * sizeof(result_last[0]) );
	gsl_sf_legendre_array(GSL_SF_LEGENDRE_SPHARM, max_l,  1.0, result_first);
	gsl_sf_legendre_array(GSL_SF_LEGENDRE_SPHARM, max_l, -1.0, result_last);
	for ( size_t j = 0; j < result_size; ++j )
	{
		size_t row_first_index = j * row_length;
		size_t row_last_index = j * row_length + ( row_length - 1 );
		plm[row_first_index] = result_first[j];
		plm[row_last_index] = result_last[j];
	}

	double* result = (double*) malloc( result_size * sizeof(result[0]) );
	double* result_deriv = (double*) malloc( result_size * sizeof(result_deriv[0]) );
	for ( size_t i = 1; i < row_length - 1; ++i )
	{
		double x = cosines->cos_theta[i];

		gsl_sf_legendre_deriv_array(GSL_SF_LEGENDRE_SPHARM, max_l, x, result, result_deriv);

		for ( size_t j = 0; j < result_size; ++j )
		{
			size_t index = j * row_length + i;
			plm[index] = result[j];
		}
	}

	*polynomials = {plm, row_length, result_size, max_l};
}

void writePolynomials(LegendrePolynomials* polys) {
	size_t size = polys->length;

	write("polynomials", "", nullptr, 0, 0, Write::CreateDirectories);
	for ( size_t polynomial = 0; polynomial < polys->number; ++polynomial )
	{
		std::cout << "Writing polynomial " << polynomial << "\n";

		std::stringstream filename{};
		filename << "polynomial-" << std::setw(3) << std::setfill('0') << polynomial;
		std::unique_ptr<double> buffer {fill_vertically(polys->plm + size * polynomial, size)};
		write("polynomials", filename.str(), buffer.get(), size, size, Write::WritePNG | Write::WriteDouble );
	}
}

int main(int argc, char* argv[])
{
	constexpr size_t image_size = 100;
	constexpr size_t max_l = 30;

	Cosines cosines;
	initializeCosines(&cosines, image_size, max_l);
	writeCosines(&cosines);

	LegendrePolynomials polys;
	initializePolynomials(&polys, &cosines, max_l, image_size);
	writePolynomials(&polys);

	std::vector<std::array<size_t, 3>> lms;

	size_t harmonic = 0;
	for ( size_t l = 0; l < max_l; ++l )
	{
		for ( size_t m = 0; m <= l; ++m )
		{
			lms.emplace_back(std::array<size_t, 3>({l, m, harmonic}));
			++harmonic;
		}
	}

	write("harmonics", "", nullptr, 0, 0, Write::CreateDirectories);
	__gnu_parallel::for_each(
			lms.begin(), lms.end(),
			[&] (const std::array<size_t, 3>& lmh) {
				size_t l = lmh[0];
				size_t m = lmh[1];
				size_t h = lmh[2];
				double* new_image = (double*) malloc(image_size * image_size * sizeof(new_image[0]));
				double* phi_image = fill_horizontally(cosines.cos_m_phi[m], image_size);
				double* poly_image = fill_vertically(polys.plm + image_size * h, image_size);

				__gnu_parallel::transform(
						phi_image, phi_image + image_size * image_size,
						poly_image,
						new_image,
						[] (double d1, double d2) -> double { return d1 * d2; });

				std::cout << "Writing harmonics for (l, m) => (" << l << ", " << m << ")\n";

				std::stringstream filename;
				filename
					<< std::setfill('0')
					<< "harmonic-"
					<< std::setw(3) << l
					<< "-"
					<< std::setw(3) << m;

				write("harmonics", filename.str(), new_image, image_size, image_size, Write::WritePNG | Write::WriteDouble);
			}
	);

	return 0;
}
