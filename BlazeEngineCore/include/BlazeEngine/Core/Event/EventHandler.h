#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"

namespace Blaze
{
	template<typename T>
	class BLAZE_API EventHandler
	{
	public:
		using EventType = T;

		virtual void OnEvent(const T& event) = 0;
	};

}
