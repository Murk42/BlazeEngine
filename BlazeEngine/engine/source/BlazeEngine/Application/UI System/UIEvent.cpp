#include "BlazeEngine/Application/UI System/UIEvent.h"

namespace Blaze::UI
{
	UIEvent::UIEvent()
		: functions()
	{
	}
	UIEvent::UIEvent(const UIEvent& e)
		: functions(e.functions)
	{
	}

	UIEvent::UIEvent(UIEvent&& e) noexcept
		: functions(std::move(e.functions))
	{
	}
	UIEvent::UIEvent(UIEventFunction const& func)
		: functions({ func})
	{

	}
	UIEvent::UIEvent(std::initializer_list<UIEventFunction> const& functions)
		: functions(functions)
	{

	}
	UIEvent::~UIEvent()
	{
		size_t count = functions.size();
		for (int i = 0; i < count; ++i)	
			functions.erase(functions.begin());
		
	}	
	void UIEvent::AddFunction(const UIEventFunction& func)
	{
		if (func)
			functions.push_back(func);
	}
	UIEvent& UIEvent::operator+=(const UIEventFunction& func)
	{		
		AddFunction(func);
		return *this;
	}
	UIEvent& UIEvent::operator+=(const UIEvent& event)
	{
		for (auto& f : event.functions)
			AddFunction(f);
		return *this;
	}	
	UIEvent& UIEvent::operator=(const UIEvent& event)
	{
		functions = event.functions;
		return *this;
	}
	UIEvent& UIEvent::operator=(UIEvent&& event) noexcept
	{
		functions = std::move(event.functions);
		return *this;
	}
}