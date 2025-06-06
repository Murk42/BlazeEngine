#pragma once
#include "BlazeEngineCore/Resource/Resource.h"
#include "BlazeEngineCore/DataStructures/StringView.h"

namespace Blaze
{
	template<typename T>
	class BLAZE_CORE_API ResourceStorage
	{
	public:
		virtual Resource<T>& CreateResource(StringView name) = 0;
		virtual Resource<T>* GetResource(StringView name) = 0;
	};	
}