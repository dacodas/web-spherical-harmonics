#include <iostream>
#include <iomanip>
#include <vector>
#include <numeric>
#include <parallel/algorithm>

#include <gsl/gsl_sf_legendre.h>
#include <png.h>



int main(int argc, char* argv[])
{
	constexpr size_t image_size = 1000;
	constexpr size_t max_l = 30;

	double* cos_theta = (double*) malloc( image_size * sizeof(cos_theta[0]) );
	double** cos_m_phi = (double**) malloc( max_l * sizeof(cos_m_phi[0]) );

	for ( size_t i = 0; i < max_l; ++i )
	{
		cos_m_phi[i] = (double*) malloc( image_size * sizeof(cos_m_phi[i][0]) );
	}

	double theta_increment = M_PI / ( image_size - 1 );
	double phi_increment = 2 * M_PI / ( image_size - 1 );
	for ( size_t i = 0; i < image_size; ++i )
	{
		cos_theta[i] = cos( i * theta_increment );
		for ( size_t m = 0; m < max_l; ++m )
		{
			cos_m_phi[m][i] = cos( m * i * phi_increment );
		}
	}

	size_t result_size = gsl_sf_legendre_array_n(max_l);
	double** p_l_m = (double**) malloc( result_size * sizeof(cos_m_phi[0]) );
	for ( size_t i = 0; i < result_size; ++i )
	{
		p_l_m[i] = (double*) malloc( image_size * sizeof(p_l_m[i][0]) );
	}

	for ( size_t i = 1; i < image_size - 1; ++i )
	{
		double x = cos_theta[i];
		double* result = (double*) malloc( result_size * sizeof(result[0]) );
		double* result_deriv = (double*) malloc( result_size * sizeof(result_deriv[0]) );

		gsl_sf_legendre_deriv_array(GSL_SF_LEGENDRE_SPHARM, max_l, x, result, result_deriv);

		for ( size_t j = 0; j < result_size; ++j )
			p_l_m[j][i] = result[j];
	}

	std::cout << p_l_m[30][40] << "\n";

	return 0;
}
