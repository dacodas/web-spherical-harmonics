std::array<float, 2> spherical(const Mesh::Point& point)
{
	const float& 
		x {point[0]},
		y {point[1]},
		z {point[2]} ;

	float xy = sqrt(pow(x, 2) + pow(y, 2));
	float theta = atan2(xy, z);
	float phi = atan2(y, x);

	if ( phi < 0.0 ) { phi += 2.0 * M_PI; }

	return std::array<float, 2> {theta, phi};
}

