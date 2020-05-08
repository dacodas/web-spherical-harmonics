#include <iostream>
#include <iomanip>
#include <vector>
#include <numeric>
#include <parallel/algorithm>

#include <gsl/gsl_sf_legendre.h>

typedef struct PixelHelper {
	size_t theta_index;
	size_t phi_index;
	double theta;
	double phi;	
} PixelHelper;

typedef struct Harmonic {
	int l;
	int m;
} Harmonic;

std::ostream& operator<<(std::ostream& out, const PixelHelper& helper) {
	out 
		<< "{" 
		<< helper.theta_index
		<< ", "
		<< helper.phi_index
		<< ", "
		<< helper.theta
		<< ", "
		<< helper.phi
		<< "}"
		;

	return out;
}

std::vector<PixelHelper> linspace_theta_phi(size_t length)
{
	std::vector<PixelHelper> helpers;
	helpers.reserve(length * length);

	auto theta = std::vector<double> (length, M_PI / length);
	auto phi = std::vector<double> (length, 2 * M_PI / length);
	std::partial_sum(theta.begin(), theta.end(), theta.begin(), std::plus<double>());
	std::partial_sum(phi.begin(), phi.end(), phi.begin(), std::plus<double>());

	for ( size_t theta_index = 0; theta_index < length; ++theta_index )
	{
		for ( size_t phi_index = 0; phi_index < length; ++phi_index )
		{
			helpers.emplace_back(PixelHelper({theta_index, phi_index, theta[theta_index], phi[phi_index]}));
		}
	}

	return helpers;
}

double rho(int l, int m, double phi, double theta)
{
	return cos(m * phi) * gsl_sf_legendre_Plm(l, m, cos(theta));
}

constexpr size_t image_size = 100;
constexpr int max_gray = 65535;
std::vector<PixelHelper> helpers = linspace_theta_phi(image_size);

Eigen::MatrixXd fill_matrix_with_spherical_harmonic(int l, int m)
{
	Eigen::MatrixXd image(image_size, image_size);
	__gnu_parallel::for_each(
			helpers.begin(), helpers.end(),
			[&] (PixelHelper& helper) {
				double _rho = rho(l, m, helper.phi, helper.theta);
				image(helper.phi_index, helper.theta_index) = _rho;

			});

	return image;
}

void write_spherical_harmonic(int l, int m)
{
	Eigen::MatrixXd image = fill_matrix_with_spherical_harmonic(l, m);
	Eigen::Matrix<uint16_t, Eigen::Dynamic, Eigen::Dynamic> int_image(image_size, image_size);

	double max = image.lpNorm<Eigen::Infinity>();

	__gnu_parallel::for_each(
			helpers.begin(), helpers.end(),
			[&] (PixelHelper& helper) {
				double _rho = image(helper.phi_index, helper.theta_index);
				int_image(helper.phi_index, helper.theta_index) = max_gray * ( 1.0 + _rho / max ) / 2.0 ;
			});

	std::stringstream ss;
	ss 
		<< "build/harmonics/harmonic-" 
		<< std::setw(3) << std::setfill('0') << l 
		<< "-" 
		<< std::setw(3) << std::setfill('0') << m
		<< ".png";

	std::string string = ss.str();

	write_png(string, image_size, int_image);
}

std::ostream& operator<<(std::ostream& out, const Harmonic& harmonic)
{
	out 
		<< "{"
		<< harmonic.l 
		<< ", "
		<< harmonic.m 
		<< "}";

	return out;
}

std::ostream& operator<<(std::ostream& out, const std::vector<Harmonic>& harmonics)
{
	for ( const auto& harmonic : harmonics )
		out << harmonic << "\n";

	return out;
}


int generate(int argc, char *argv[])
{
	constexpr int max_harmonic = 30;
	size_t total_harmonics = max_harmonic * ( max_harmonic + 1 ) / 2;
	std::vector<Harmonic> harmonics;
	harmonics.reserve(total_harmonics);

	for ( int l = 0; l <= max_harmonic; ++l )
		for ( int m = 0; m <= l; ++m )
			harmonics.emplace_back(Harmonic{l, m});

	std::cout << harmonics;

 	__gnu_parallel::for_each(
 			harmonics.begin(),
 			harmonics.end(),
 			[&] (Harmonic& harmonic) {
 			std::cout << "Writing " << harmonic << "\n";
 			write_spherical_harmonic(harmonic.l, harmonic.m);
 			});

	return 0;
}

