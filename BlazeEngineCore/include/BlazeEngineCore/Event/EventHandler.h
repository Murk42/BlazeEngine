#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"

namespace Blaze
{		
	template<typename T>
	class BLAZE_CORE_API EventHandler
	{
	public:
		using EventType = T;		
		
		virtual void OnEvent(const T& event) = 0;		
	};
			
}