#include <numeric>
#include <algorithm>

#include "fill-and-convert.h"

uint16_t convert(double d, double max)
{
	return 65535 * ( 1.0 + d / max ) / 2.0;
}

uint16_t* toUint16(double* buffer, size_t size, double max) 
{
	uint16_t* _row = (uint16_t*) malloc( size * sizeof(_row[0]) );

	std::transform(
			buffer, buffer + size, _row, 
			[&max] (double d) -> uint16_t { return convert(d, max); }
		      );

	return _row;
}

double max_abs(double* buffer, size_t size)
{
	return std::transform_reduce(
			buffer, buffer + size, 0.0d, 
			[] (double a, double b) -> double { return std::max(a, b); }, 
			[] (double d) -> double { return std::abs(d); });
}

uint16_t* toUint16(double* buffer, size_t size) 
{
	double max = max_abs(buffer, size);
	return toUint16(buffer, size, max);

}
