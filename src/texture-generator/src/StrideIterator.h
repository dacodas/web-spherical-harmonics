template<typename T>
class StrideIterator {
	private:
		size_t row_position;
		size_t row_size;
		size_t stride_size;
		T* _it;

	public:

		using difference_type = size_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::input_iterator_tag;

		StrideIterator<T>(T* it, size_t row_size, size_t stride_size)
			: _it {it},
			row_position {0},
			row_size {row_size},
			stride_size {stride_size} {}

		// T operator*()
		// {
		// 	return *_it;
		// }

		T& operator*()
		{
			return *_it;
		}

		StrideIterator<T> operator++()
		{
			if ( row_position == row_size - 1 )
			{
				_it -= row_position;
				_it += stride_size;
				row_position = 0;
			}
			else
			{
				++_it;
				++row_position;
			}
			return *this;
		}

		friend bool operator!=( const StrideIterator<T> lhs, const StrideIterator<T> rhs )
		{
			return lhs._it != rhs._it;
		}

		friend StrideIterator<T> operator+(StrideIterator<T> lhs, int rhs)
		{
			for ( size_t i = 0 ; i < rhs ; ++i )
			{
				++lhs;
			}
			return lhs;
		}
};

