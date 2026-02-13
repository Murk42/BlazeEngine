#pragma once
#include "BlazeEngine/Core/String/StringView.h"

namespace Blaze
{
	template<typename T>
	class ResourceRef;

	template<typename T>
	class BLAZE_API ResourceStorage
	{
	public:
		virtual ResourceRef<T> CreateResource(StringView name) = 0;
		virtual ResourceRef<T> GetResource(StringView name) = 0;
	};	
}