#pragma once

namespace Blaze
{
	template<typename T>
	class ArrayView
	{
		T* ptr;
		unsigned size;
	public:
		ArrayView() : ptr(nullptr), size(0) { }

		unsigned Size() const { return size; }
		T* Ptr() { return ptr; }
		const T* Ptr() const { return ptr; }

		T& operator[](unsigned i) { return ptr[i]; }
		const T& operator[](unsigned i) const { return ptr[i]; }
	};
}