#pragma once
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

template<typename T>
class DynamicMatrix
{
	std::vector<T> arr;
public:
	Vec2i size;

	void Resize(Vec2i newSize)
	{
		size = newSize;
		arr.resize(size.x * size.y);
	}

	T* operator[](int x)
	{
		return arr.data() + x * size.y;
	}
};