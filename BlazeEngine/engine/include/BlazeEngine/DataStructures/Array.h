#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include <type_traits>
#include <cstring>
#include <vector>

namespace Blaze
{
	template<typename T>
	class BLAZE_API Array
	{
		T* ptr;
		size_t size;
	public:
		Array()
			: ptr(nullptr), size(0)
		{
		}
		~Array()
		{
			delete[] ptr;
		}

		void Resize(size_t newSize)
		{
			T* old = ptr;
			ptr = new T[newSize];

			size_t smaller = std::minus(newSize, size);

			if (std::is_trivially_copyable_v<T>)
				memcpy(ptr, old, sizeof(T) * smaller);
			else for (int i = 0; i < smaller; ++i)
				ptr[i] = old[i];

			delete[] old;

			size = newSize;
		}

		size_t Size() const { return size; }

		T& operator[](size_t i)
		{
			if (i >= size)
				throw std::exception("index out of bounds");

			return ptr[i];
		}
		const T& operator[](size_t i) const
		{
			if (i >= size)
				throw std::exception("index out of bounds");

			return ptr[i];
		}

		T* begin() const { return ptr; }
		T* end() const { return ptr + size; }
	};
}