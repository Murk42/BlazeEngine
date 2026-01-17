#include "ReferenceContainer.h"
#pragma once

namespace Blaze
{
	template<typename T> requires (!IsReferenceType<T>)
	inline ReferenceContainer<T>::ReferenceContainer(T& value)
		: ptr(&value)
	{
	}
	template<typename T> requires (!IsReferenceType<T>)
	inline T* ReferenceContainer<T>::Ptr() const
	{
		return ptr;
	}
	template<typename T> requires (!IsReferenceType<T>)
	inline ReferenceContainer<T>::operator T& () const
	{
		return *ptr;
	}
	template<typename T> requires (!IsReferenceType<T>)
	inline ReferenceContainer<T>::operator ReferenceContainer<T>() const
	{
		return ReferenceContainer<T>(*ptr);
	}
}
