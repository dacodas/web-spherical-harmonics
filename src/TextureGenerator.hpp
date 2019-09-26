#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <functional>

#include <parallel/algorithm>
#include <parallel/numeric>
#include <gsl/gsl_sf_legendre.h>

typedef struct Harmonic
{
    int l;
    int m;
    double amplitude;
    double phase;
} Harmonic;

class TextureGenerator
{
public:

    TextureGenerator(std::vector<Harmonic>&& harmonics);
    void calculate();
    void write_textures();

    std::filesystem::path rho_path;
    std::filesystem::path normals_path;

private: 

    int steps;
    bool write_file;
    std::vector<double> theta;
    std::vector<double> phi;
    std::vector<Harmonic> harmonics;

    std::vector<double> precise_rho_values;

    std::vector<int> rho_values;
    std::vector<std::array<int, 3>> normals_values;

    std::string identifier;
    std::filesystem::path output_directory; 

    void write_pgm(std::ostream& file, const std::function<void(std::ostream&, size_t)>& pixel_writer);
    void write_pgm(const std::vector<std::array<int, 3>>& picture, std::filesystem::path& file);
    void write_pgm(const std::vector<int>& picture, std::filesystem::path& file);

    void set_identifier();

    void linspace_theta_phi();
    void calculate_rho();
    void calculate_normals();

};

// std::filesystem::path normals_path {
//     output_path.parent_path().string()
//     + "/"
//     + output_path.stem().string()
//     + "-normals.pgm"
// };

// std::filesystem::path output_path {
//     "/usr/local/apache2/htdocs/images/rho-"
//     + std::to_string(component.l)
//     + "_"
//     + std::to_string(component.m)
//     + ".pgm"
// };
