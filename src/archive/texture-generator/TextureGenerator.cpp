#include <iomanip>

#include "TextureGenerator.hpp"
#include "convert.h"

TextureGenerator::TextureGenerator(std::vector<Harmonic>&& harmonics)
{
    this->harmonics = harmonics;
    this->steps = 100;
    this->write_file = true;

    size_t image_size = pow(steps, 2);

    this->precise_rho_values = std::vector<double>(image_size);
    this->rho_values = std::vector<int>(image_size);
    this->normals_values = std::vector<std::array<int, 3>>(image_size);

    this->output_directory = std::filesystem::path {"/usr/local/apache2/htdocs/images/"};
    set_identifier();
}

void TextureGenerator::calculate()
{
    linspace_theta_phi();
    calculate_rho();
    calculate_normals();
}

void TextureGenerator::write_textures()
{
    auto rho_path_pgm = std::filesystem::path { this->output_directory.string() + "rho-" + identifier + ".pgm"};
    auto normals_path_pgm = std::filesystem::path { this->output_directory.string() + "normals-" + identifier + ".pgm"};

    write_pgm(rho_values, rho_path_pgm);
    write_pgm(normals_values, normals_path_pgm);
    convert(rho_path_pgm);
    convert(normals_path_pgm);

    this->rho_path = std::filesystem::path { this->output_directory.string() + "rho-" + identifier + ".png"};
    this->normals_path = std::filesystem::path { this->output_directory.string() + "normals-" + identifier + ".png"};
}

void write_harmonic(const Harmonic& harmonic, std::ostream& stream)
{
    stream
        << harmonic.l << "-"
        << harmonic.m << "-"
        << std::setprecision(2)
        << std::fixed
        << harmonic.amplitude;
}

void TextureGenerator::set_identifier()
{
    std::ostringstream identifier_stream {};

    write_harmonic(harmonics[0], identifier_stream);
    std::for_each(
        ++harmonics.begin(),
        harmonics.end(),
        [&] (const Harmonic& harmonic)
            {
                identifier_stream << "-";
                write_harmonic(harmonic, identifier_stream);
            }
        );

    this->identifier = identifier_stream.str();
}


void TextureGenerator::linspace_theta_phi()
{
    theta = std::vector<double> (steps, M_PI / steps);
    phi = std::vector<double> (steps, 2 * M_PI / steps);
    std::partial_sum(theta.begin(), theta.end(), theta.begin(), std::plus<double>());
    std::partial_sum(phi.begin(), phi.end(), phi.begin(), std::plus<double>());
}

void TextureGenerator::calculate_rho()
{
    double* theta_begin = &( *theta.begin() );
    double* phi_begin = &( *phi.begin() );

    __gnu_parallel::for_each(
        theta.begin(),
        theta.end(),
        [&] (double& current_theta) {

            size_t theta_index = &current_theta - theta_begin;

            __gnu_parallel::for_each(
                phi.begin(), phi.end(),
                [&] (double& current_phi)
                    {
                        double rho;
                        size_t phi_index = &current_phi - phi_begin;

                        std::vector<double> calculated_components(harmonics.size(), 0.0);
                        __gnu_parallel::transform(
                            harmonics.begin(),
                            harmonics.end(),
                            calculated_components.begin(),
                            [&] (const Harmonic& component) -> double
                                {
                                    double rho = \
				    component.amplitude \
				    	* cos(component.m * current_phi) \
					* gsl_sf_legendre_sphPlm( \
							component.l, \
							component.m, \
							cos(current_theta));
                                    return rho;
                                }
                            );

                        rho = __gnu_parallel::accumulate(calculated_components.begin(),
                                                         calculated_components.end(),
                                                         0.0) / calculated_components.size();

                        int rho_int = ( 255.0 / 2.0 ) * ( rho + 1.0 );

                        precise_rho_values[theta_index * steps + phi_index] = rho;
                        rho_values[theta_index * steps + phi_index] = rho_int;
                    });
        });
}

void TextureGenerator::calculate_normals()
{
    std::vector<int> range(steps);
    std::iota(range.begin(), range.end(), 0);

    normals_values = std::vector<std::array<int, 3>>(precise_rho_values.size());

    __gnu_parallel::for_each(
        range.begin(),
        range.end(),
        [&] (const int& row)
            {
                __gnu_parallel::for_each(
                    range.begin(),
                    range.end(),
                    [&] (const int& column)
                        {

                            int index = row * steps + column;
                            double x_diff = column == steps - 1 ? precise_rho_values[index - ( steps - 1 )] - precise_rho_values[index] : precise_rho_values[index + 1] - precise_rho_values[index];
                            double y_diff = row == steps - 1 ? 0.0 : precise_rho_values[index + steps] - precise_rho_values[index];

                            double theta = - 2 * M_PI * x_diff / 100.0;
                            double phi = M_PI * y_diff / 100.0;
                            double rho = 2 * pow(M_PI, 2.0) / pow(100.0, 2.0);

                            double magnitude = pow(pow(theta, 2) + pow(phi, 2) + pow(rho, 2), 0.5);

                            theta /= magnitude;
                            phi /= magnitude;
                            rho /= magnitude;

                            // Cross product
                            normals_values[index][0] = ( theta + 1.0 ) * ( 255 / 2 );
                            normals_values[index][1] = ( phi + 1.0 ) * ( 255 / 2 );
                            normals_values[index][2] = ( rho + 1.0 ) * ( 255 / 2 );
                        }
                    );
            }
        );
}

void TextureGenerator::write_pgm(std::ostream& file, const std::function<void(std::ostream&, size_t)>& pixel_writer)
{
    std::cout << "Writing file...\n";
    file << "P3\n"
         << "# \n"
         << steps << " " << steps << "\n"
         << "255\n";

    for ( size_t i = 0; i < pow(steps, 2); ++i )
    {
        pixel_writer(file, i);
        if ( ( i + 1 ) % steps == 0 )
        {
            file << "\n";
        }
    }
}

void TextureGenerator::write_pgm(const std::vector<std::array<int, 3>>& picture, std::filesystem::path& path)
{
    std::ofstream file {path};
    write_pgm(file, [&] (std::ostream& file, size_t i)
                          {
                              file << picture[i][0] << " " << picture[i][1] << " " << picture[i][2] << " ";
                          }
        );
}

void TextureGenerator::write_pgm(const std::vector<int>& picture, std::filesystem::path& path)
{
    std::ofstream file {path};
    write_pgm(file, [&] (std::ostream& file, size_t i)
                          {
                              file << picture[i] << " 0 0 ";
                          }
        );
}
