#include <string>

#include <png.h>
#include <eigen3/Eigen/Dense>

void write_png(
const std::string& filename, 
uint16_t* image,
size_t image_size
);

void write_png(
const std::string& filename, 
uint16_t* image,
size_t row_size,
size_t row_number
);

void write_png(
const std::string& filename, 
Eigen::Matrix<uint16_t, Eigen::Dynamic, Eigen::Dynamic>& image,
size_t image_size
);
