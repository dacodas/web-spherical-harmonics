#include <filesystem>
#include <regex>

#include <parallel/algorithm>
#include <parallel/numeric>

#include <fcgi_stdio.h>

#include "TextureGenerator.hpp"
#include "convert.h"

std::pair<bool, std::vector<Harmonic>> validate_input(std::istringstream& input_stream)
{
    std::vector<Harmonic> harmonics {};

    while ( input_stream.good() )
    {
        Harmonic component {};

        input_stream >> component.l;
        if ( input_stream.fail() )
        {
            printf("Invalid input\n");
            return {false, {}};
        }

        input_stream >> component.m;
        if ( input_stream.fail() or component.m > component.l )
        {
            printf("Invalid input\n");
            return {false, {}};
        }

        input_stream >> component.amplitude;
        if ( input_stream.fail() or component.amplitude > 1.0 or component.amplitude < 0.0)
        {
            printf("Invalid input\n");
            return {false, {}};
        }
                
        input_stream >> component.phase;
        if ( input_stream.fail() )
        {
            printf("Invalid input\n");
            return {false, {}};
        }

        harmonics.emplace_back(std::move(component));
    }

    return {true, harmonics};
}

int main(int argc, char* argv[])
{
    char* query_string = nullptr;

    while ( FCGI_Accept() >= 0 )
    {
        printf(
            "Content-type: text/html\r\n"
            "\r\n"
            );

        query_string = getenv("HTTP_X_TEXTURE_COMPONENTS");
        if ( query_string == nullptr )
        {
            printf("X-Texture-Components header not set...\n");
            continue;
        }

        std::istringstream input_stream {query_string};
        auto&& [successful, harmonics] = validate_input(input_stream);

        if ( !successful )
            continue;

        TextureGenerator texture_generator(std::move(harmonics));
        texture_generator.calculate();
        texture_generator.write_textures();

        printf("[\"%s\", \"%s\"]\n",
               std::regex_replace(
                   texture_generator.rho_path.string(),
                   std::regex("/usr/local/apache2/htdocs"),
                   "/spherical-harmonics").c_str(),
               std::regex_replace(
                   texture_generator.normals_path.string(),
                   std::regex("/usr/local/apache2/htdocs"),
                   "/spherical-harmonics").c_str());

    }

    return 0;
}
