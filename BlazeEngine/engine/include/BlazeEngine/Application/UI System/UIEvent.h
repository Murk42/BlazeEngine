#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include <vector>
#include <functional>

namespace Blaze::UI
{
	class BLAZE_API UIEvent
	{
		std::vector<std::function<void()>> functions;
	public:
		UIEvent();
		~UIEvent();

		void CallEvent() const;

		void AddFunction(const std::function<void()>& func);
		UIEvent& operator+=(const std::function<void()>& func);		
		void operator() () const;
	};
}