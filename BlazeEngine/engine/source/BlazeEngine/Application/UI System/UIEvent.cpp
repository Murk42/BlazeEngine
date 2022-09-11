#include "BlazeEngine/Application/UI System/UIEvent.h"

namespace Blaze::UI
{
	UIEvent::UIEvent()
		: functions()
	{
	}
	UIEvent::~UIEvent()
	{
		functions.clear();
	}
	void UIEvent::CallEvent() const
	{
		for (auto& f : functions)
			f();
	}
	void UIEvent::AddFunction(const std::function<void()>& func)
	{
		if (func)
			functions.push_back(func);
	}
	UIEvent& UIEvent::operator+=(const std::function<void()>& func)
	{		
		AddFunction(func);
		return *this;
	}
	void UIEvent::operator() () const
	{
		CallEvent();		
	}
}