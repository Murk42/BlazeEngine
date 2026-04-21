#pragma once
#include "BlazeEngine/Core/String/StringView.h"

namespace Blaze
{
	template<typename T>
	class ResourceRef;
	template<typename T>
	class Resource;

	template<typename T>
	class BLAZE_API ResourceStorage
	{
	public:
		virtual ResourceRef<T> AllocateNamedResource(StringView name) = 0;
		virtual ResourceRef<T> AllocateResource() = 0;
		virtual ResourceRef<T> GetNamedResource(StringView name) = 0;
	protected:
		virtual void FreeResource(Resource<T>& resource) = 0;
		
		friend class Resource<T>;
	};	
}