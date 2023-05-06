#pragma once
#include <utility>

namespace Blaze
{
	template<typename T>
	class ArrayView
	{
		const T* ptr;
		size_t count;
	public:
		using value_type = T;

		ArrayView()
			: ptr(nullptr), count(0)
		{
		}
		ArrayView(T* ptr, size_t count)
			: ptr(ptr), count(count)
		{
		}
		ArrayView(const ArrayView& arr)
			: ptr(arr.ptr), count(arr.count)
		{
		}
		template<typename A>
		ArrayView(const Array<T, A>& arr)
			: ptr(arr.Ptr()), count(arr.Count())
		{
		}		

		size_t Count() const { return count; }
		const T* Ptr() const { return ptr; }

		const T& operator[](size_t index) const
		{
			if (index >= count)
				throw;
			return ptr[index];
		}

		ArrayView operator=(const ArrayView& arr)
		{
			ptr = arr.ptr;
			count = arr.count;
		}
		ArrayView operator=(const Array<T>& arr)
		{
			ptr = arr.Ptr();
			count = arr.Count();
		}

		const T* begin() const { return ptr; }
		const T* end() const { return ptr + count; }
	};

	template<typename T, typename A>
	Array<T, A>::Array(const ArrayView<T>& arr)
		: Array(arr.Ptr(), arr.Count())
	{
	}
	template<typename T, typename A>
	Array<T, A>& Array<T, A>::operator=(const ArrayView<T>& arr)
	{
		*this = std::move(Array(arr.Ptr(), arr.Count()));
		return *this;
	}
}