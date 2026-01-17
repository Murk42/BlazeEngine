#pragma once
#include "BlazeEngine/Core/Type/TypeTraits.h"

namespace Blaze
{
	template<typename T> requires (!IsReferenceType<T>)
	class ReferenceContainer
	{
	public:
		ReferenceContainer(T& value);
		ReferenceContainer(const ReferenceContainer& other) = default;

		T* Ptr() const;

		operator T& () const;
		operator ReferenceContainer<T>() const;

		bool operator==(const ReferenceContainer& other) const = default;
		bool operator!=(const ReferenceContainer& other) const = default;
	private:
		T* ptr;
	};
}

#include "BlazeEngine/Core/Container/ReferenceContainerImpl.h"