#pragma once
#include "BlazeEngineCore/Resource/ResourceRef.h"
#include "BlazeEngineCore/DataStructures/StringView.h"

namespace Blaze
{
	template<typename T>
	class BLAZE_CORE_API ResourceStorage
	{
	public:
		virtual ResourceRef<T> CreateResource(StringView name) = 0;
		virtual ResourceRef<T> GetResource(StringView name) = 0;
	};	
}