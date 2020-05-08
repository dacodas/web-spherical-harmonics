# Fix memory issues and this will likely be fine

Some old code from use-gsl-array that I'd like to profile vs. current

```
// Archive, check for later
//	for ( size_t harmonic = 0; harmonic < result_size; ++harmonic )
//	{
//		Eigen::Map<Eigen::Matrix<double, image_size, 1>> eigen_plm(p_l_m[harmonic]);
//		double max = eigen_plm.lpNorm<Eigen::Infinity>();
//
//		double* image = (double*) malloc( image_size * image_size * sizeof(image[0]) );
//		uint16_t* int_image = (uint16_t*) malloc( image_size * image_size * sizeof(int_image[0]) );
//		for ( size_t i = 0; i < image_size; ++i )
//		{
//			double* row_start = image + image_size * i;
//			memcpy(row_start, p_l_m[harmonic], image_size * sizeof(double));
//			for ( size_t j = 0; j < image_size; ++j )
//			{ 
//				size_t index = i * image_size + j;
//				int_image[index] = 65535 * ( 1.0 + image[index] / max ) / 2.0; 
//			}
//		}
//
//		std::cout << "Just to check, the max was " << max << "\n";
//		std::stringstream ss {};
//		ss << "build/harmonic-" << std::setw(3) << std::setfill('0') << harmonic << ".png";
//
//		write_png(ss.str(), int_image, image_size);
//	}
```
