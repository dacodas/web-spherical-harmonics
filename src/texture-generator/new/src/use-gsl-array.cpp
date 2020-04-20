#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <vector>
#include <array>

#include <parallel/algorithm>

#include <gsl/gsl_sf_legendre.h>
#include "png.h"

uint16_t convert(double d, double max)
{
	return 65535 * ( 1.0 + d / max ) / 2.0;
}

uint16_t* toUint16(double* row, size_t size) 
{
	uint16_t* _row = (uint16_t*) malloc( size * sizeof(_row[0]) );

	double max = std::transform_reduce(
			row, row + size, 0.0d, 
			[] (double a, double b) -> double { return std::max(a, b); }, 
			[] (double d) -> double { return std::abs(d); });

	std::transform(
			row, row + size, _row, 
			[&max] (double d) -> uint16_t { return convert(d, max); }
		      );

	return _row;
}

template<typename T>
T* fill_horizontally(T* vector, size_t size) {
	T* image = (T*) malloc( size * size * sizeof(image[0]) );
	for ( size_t i = 0; i < size; ++i )
	{
		T* dest = image + size * i;
		memcpy(dest, vector, size * sizeof(T));
	}
	return image;
}

template<typename T>
T* fill_vertically(T* vector, size_t size) {
	T* transpose = fill_horizontally(vector, size);
	Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>> transpose_map(transpose, size, size);
	transpose_map.transposeInPlace();
	return transpose;
}

typedef struct Cosines {
	double* cos_theta;
	double** cos_m_phi;
	size_t row_size;
	size_t max_l;
} Cosines;

void writeCosines(Cosines* cosines) {
	size_t size = cosines->row_size;

	uint16_t* cos_theta_uint16_t = toUint16(cosines->cos_theta, size);
	uint16_t* cos_theta_image = fill_horizontally(cos_theta_uint16_t, size);
	write_png("build/cos-theta.png", cos_theta_image, size);

	for ( size_t m = 0; m < cosines->max_l; ++m )
	{
		uint16_t* cos_m_phi_uint16_t = toUint16(cosines->cos_m_phi[m], size);
		uint16_t* cos_m_phi_image = fill_vertically(cos_m_phi_uint16_t, size);

		std::stringstream cos_m_phi_output_filename {};
		cos_m_phi_output_filename << "build/cos-m-phi-" << std::setw(3) << std::setfill('0') << m << ".png";
		write_png(cos_m_phi_output_filename.str(), cos_m_phi_image, size);

		std::cout << "Writing cosine " << m << "\n";
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

typedef struct LegendrePolynomials {
	double* plm;
	size_t length;
	size_t number;
	size_t max_l;
} LegendrePolynomials;

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

	for ( size_t harmonic = 0; harmonic < polys->number; ++harmonic )
	{
		uint16_t* int_vector = toUint16(polys->plm + size * harmonic, size);
		uint16_t* int_image = fill_vertically(int_vector, size);

		std::stringstream filename{};
		filename << "build/harmonic-" << std::setw(3) << std::setfill('0') << harmonic << ".png";
		write_png(filename.str(), int_image, size);

		std::cout << "Writing polynomial " << harmonic << "\n";
	}
}

int main(int argc, char* argv[])
{
	constexpr size_t image_size = 250;
	constexpr size_t max_l = 100;

	Cosines cosines;
	initializeCosines(&cosines, image_size, max_l);
	// writeCosines(&cosines);

	LegendrePolynomials polys;
	initializePolynomials(&polys, &cosines, max_l, image_size);
	// writePolynomials(&polys);

//	double* phi_images = (double*) malloc(max_l * image_size * image_size * sizeof(phi_images[0]));
//	double* poly_images = (double*) malloc(polys.number * image_size * image_size * sizeof(poly_images[0]));
//
//	for ( size_t i = 0; i < polys.number; ++i )
//	{
//		std::cout << i << "\n";
//		double* womp = fill_vertically(polys.plm + image_size * i, image_size);
//		memcpy(poly_images + i * image_size * image_size, womp, image_size * image_size * sizeof(poly_images[0]));
//		free(womp);
//	}
//
//	for ( size_t m = 0; m < max_l; ++m )
//	{
//		std::cout << m << "\n";
//		double* womp = fill_horizontally(cosines.cos_m_phi[m], image_size);
//		memcpy(phi_images + m * image_size * image_size, womp, image_size * image_size * sizeof(phi_images[0]));
//		free(womp);
//	}
//
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

				uint16_t* new_image_int = toUint16(new_image, image_size * image_size);

				std::stringstream filename; 
				filename 
				<< std::setfill('0')
				<< "build/actual-harmonic-" 
				<< std::setw(3) << l 
				<< "-" 
				<< std::setw(3) << m
				<< ".png";
				std::cout << "Writing " << filename.str() << "\n";
				write_png(filename.str(), new_image_int, image_size);
			}
	);

	//	for ( size_t harmonic = 0; harmonic < result_size; ++harmonic )
	//	{
	//		Eigen::Map<Eigen::Matrix<double, image_size, 1>> eigen_plm(p_l_m[harmonic]);
	//		double max = eigen_plm.lpNorm<Eigen::Infinity>();
	//
	//		double* image = (double*) malloc( image_size * image_size * sizeof(image[0]) );
	//		uint16_t* int_image = (uint16_t*) malloc( image_size * image_size * sizeof(int_image[0]) );
	//		for ( size_t i = 0; i < image_size; ++i )
	//		{
	//			double* row_start = image + image_size * i;
	//			memcpy(row_start, p_l_m[harmonic], image_size * sizeof(double));
	//			for ( size_t j = 0; j < image_size; ++j )
	//			{ 
	//				size_t index = i * image_size + j;
	//				int_image[index] = 65535 * ( 1.0 + image[index] / max ) / 2.0; 
	//			}
	//		}
	//
	//		std::cout << "Just to check, the max was " << max << "\n";
	//		std::stringstream ss {};
	//		ss << "build/harmonic-" << std::setw(3) << std::setfill('0') << harmonic << ".png";
	//
	//		write_png(ss.str(), int_image, image_size);
	//	}

	return 0;
}
