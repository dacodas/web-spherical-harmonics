#include <iostream>
#include <iomanip>
#include <vector>
#include <numeric>
#include <parallel/algorithm>

#include <gsl/gsl_sf_legendre.h>
#include <png.h>
#include <Eigen/Dense>

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

int write_png(const std::string& filename, size_t image_size, Eigen::Matrix<uint16_t, Eigen::Dynamic, Eigen::Dynamic>& image)
{
	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png) return 1;

	png_infop info = png_create_info_struct(png);
	if (!info) return 1;

	FILE* fp = fopen(filename.c_str(), "wb");
	png_init_io(png, fp);
	png_set_IHDR(
			png, 
			info,
			image_size, image_size,
			16,
			PNG_COLOR_TYPE_GRAY,
			PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT,
			PNG_FILTER_TYPE_DEFAULT
		    );
	png_write_info(png, info);
	png_set_swap(png);

	short unsigned int* data = image.data();
        if (setjmp(png_jmpbuf(png)))
                printf("[write_png_file] Error during writing bytes");

	for ( size_t i = 0; i < image_size; ++i )
	{
		auto row_start = image.data() + image_size * i;
		png_write_row(png, (png_const_bytep) row_start);
	}
	png_write_end(png, NULL);

	png_destroy_write_struct(&png, &info);
	fclose(fp);

	return 0;
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

int serialize(int argc, char *argv[])
{
	Eigen::MatrixXd image = fill_matrix_with_spherical_harmonic(5, 5);
	auto data = image.data();

	size_t size_in_octets = image_size * image_size * sizeof(double);

	for ( size_t i = 0 ; i < size_in_octets ; ++i )
	{
		std::cout << ((uint8_t*) data)[i];
	}	

	return 0;
}

int deserialize(int argc, char *argv[])
{
	return 0;
}

int check_double(int argc, char* argv[])
{
	uint8_t data[] {0x2a, 0x8b, 0xc2, 0x2e, 0xfe, 0x60, 0xea, 0x40};
	double d {*reinterpret_cast<double*>(&data)};

	std::cout << std::setprecision(20) << d << "\n";
		
	return 0;
}

int print_double(int argc, char* argv[])
{
	// Ensure that doubles adhere to IEEE 754 
	
	double d = 54023.943208;
	for ( size_t i = 0; i < 8 ; ++i )
	{
		std::cout 
			<< std::hex 
			<< static_cast<int>(reinterpret_cast<uint8_t*>(&d)[i])
			<< " ";
	}
	std::cout << "\n";

	return 0;
}

int print_double_array(int argc, char* argv[])
{
	// Ensure that doubles adhere to IEEE 754 
	
	double d[] {
		54023.943208,
		44564564.4464343575,
		0.46842135452,
		4654121.7756543
	};

	size_t number_of_doubles = sizeof(d) / sizeof(d[0]);
	size_t number_of_octets = 8 * number_of_doubles;

	for ( size_t i = 0; i < number_of_octets ; ++i )
	{
		std::cout 
			<< std::hex 
			<< static_cast<int>(reinterpret_cast<uint8_t*>(&d)[i])
			<< " ";
	}
	std::cout << "\n";

	return 0;
}

int check_double(int argc, char* argv[])
{
	// If we are using POSIX, CHAR_BIT == 8
	//
	// Using uint8_t is guaranteed to be possible 
	
	uint8_t data[] {0x2a, 0x8b, 0xc2, 0x2e, 0xfe, 0x60, 0xea, 0x40};
	double d {*reinterpret_cast<double*>(&data)};

	size_t number_of_octets = sizeof(data) / ;

	std::cout << std::setprecision(20) << d << "\n";
		
	return 0;
}

int main(int argc, char* argv[])
{
	if ( argc < 2 )
	{
		std::cout 
			<< "Specify subcommand among: " << "\n"
			// $SUBCOMMANDS
			;
		return(1);
	}

	// $PROCESS_ARGUMENTS

	std::cout << "Unrecognized subcommand" << "\n";
	return 1;
}
