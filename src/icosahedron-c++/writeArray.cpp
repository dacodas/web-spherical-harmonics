template <typename T, size_t size>
std::ostream& operator<<(std::ostream& out, const std::array<T, size>& v)
{
	size_t i = 0;
	for ( ; i < size - 1; ++i )
	{
		out << v[i] << " ";
	}
	out << v[i];

	return out;
}
