#pragma once
#include "BlazeEngine/Event/Events.h"

namespace Blaze
{
	template<typename T>
	class EventHandler
	{
	public:
		virtual void OnEvent(T event) = 0;
	};
}