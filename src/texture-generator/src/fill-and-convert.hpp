template<typename T>
T* fill_horizontally(T* vector, size_t size) {
	T* image = (T*) malloc( size * size * sizeof(image[0]) );
	for ( size_t i = 0; i < size; ++i )
	{
		T* dest = image + size * i;
		memcpy(dest, vector, size * sizeof(T));
	}
	return image;
}

template<typename T>
T* fill_vertically(T* vector, size_t size) {
	T* toTranspose = fill_horizontally(vector, size);
	return transpose(toTranspose, size);
}

template<typename T>
T* transpose(T* matrix, size_t size) {
	Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>> transpose_map(matrix, size, size);
	transpose_map.transposeInPlace();
	return matrix;
}

template<typename T>
T* transpose(T* matrix, size_t major_size, size_t minor_size) {
	Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>> transpose_map(matrix, major_size, minor_size);
	transpose_map.transposeInPlace();
	return matrix;
}

template<typename T>
T* fill_horizontally(T* vector, size_t major_size, size_t minor_size)
{
	T* image = (T*) malloc( major_size * minor_size * sizeof(image[0]) );
	for ( size_t i = 0; i < minor_size; ++i )
	{
		T* dest = image + major_size * i;
		memcpy(dest, vector, major_size * sizeof(T));
	}
	return image;
}

template<typename T>
T* fill_vertically(T* vector, size_t major_size, size_t minor_size)
{
	T* toTranspose = fill_horizontally(vector, major_size, minor_size);
	return transpose(toTranspose, major_size, minor_size);
}
