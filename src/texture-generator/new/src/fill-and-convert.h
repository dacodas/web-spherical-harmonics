#include <eigen3/Eigen/Dense>


uint16_t convert(double d, double max);
uint16_t* toUint16(double* buffer, size_t size);

template<typename T>
T* fill_horizontally(T* vector, size_t size);

template<typename T>
T* fill_vertically(T* vector, size_t size);

template<typename T>
T* transpose(T* matrix, size_t size);

template<typename T>
T* fill_horizontally(T* vector, size_t major_size, size_t minor_size);

template<typename T>
T* fill_vertically(T* vector, size_t major_size, size_t minor_size);

#include "fill-and-convert.hpp"
