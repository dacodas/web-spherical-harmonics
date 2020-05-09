#include <vector>
#include <algorithm>
#include <iostream>

#include "png.cpp"
#include "texture-generator/src/StrideIterator.h"

typedef struct Pixel {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
} Pixel;

int main()
{
	constexpr size_t size {1000};
	constexpr size_t every_n_pixels = 33;
	std::vector<Pixel> image(size * size, Pixel {0, 0, 0, 0});

	for ( size_t i = 0; i < size / every_n_pixels; ++i )
	{ 
		for ( size_t j = 0 ; j < 3; ++j ) 
		{
			auto start = image.begin() + (i * every_n_pixels + j) * size;
			auto end = start + size;
			std::transform(
					start, 
					end,
					start, 
					[] (const Pixel& pixel) -> Pixel 
					{
					return {0, 0, 0, 255};
					}
				      );

			StrideIterator<Pixel> _start(image.data() + i * every_n_pixels + j, 1, size);
			StrideIterator<Pixel> _end = _start + size;

			for ( StrideIterator<Pixel> it = _start ; it != _end ; ++it )
			{
				std::cout << ( &*it - image.data() ) << "\n";
				std::cout << std::distance(_start, it ) << "\n";
				// std::cout << (int) (*it).a << "\n";
			}

			std::transform(
					_start, 
					_end,
					_start, 
					[] (const Pixel& pixel) -> Pixel 
					{
					return {0, 0, 0, 255};
					}
				      );
		}
	}

	write_png(size, size, reinterpret_cast<uint8_t*>(image.data()), "output");

	return 0;
}
