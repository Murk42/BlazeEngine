#include "pch.h"
#include "BlazeEngine/Runtime/IO/Input.h"
#include "BlazeEngine/UI/Common/ButtonBase.h"
#include "BlazeEngine/UI/Input/InputSubSystem.h"

namespace Blaze::UI
{
	ButtonBase::ButtonBase()
		: pressingMouseID(UINT32_MAX), pressable(true)
	{
		//SetMouseMotionReportPolicy(MouseMotionReportPolicy::ReportWhileSelected);

		mouseButtonDownEventDispatcher.AddHandler<&ButtonBase::MouseButtonDownEvent>(*this);
		mouseButtonUpEventDispatcher.AddHandler<&ButtonBase::MouseButtonUpEvent>(*this);
		selectedStateChangedEventDispatcher.AddHandler<&ButtonBase::SelectedStateChangedEvent>(*this);
		mouseHitStatusChangedEventDispatcher.AddHandler<&ButtonBase::HitStatusChangedEvent>(*this);
	}
	ButtonBase::~ButtonBase()
	{
		mouseButtonDownEventDispatcher.RemoveHandler<&ButtonBase::MouseButtonDownEvent>(*this);
		mouseButtonUpEventDispatcher.RemoveHandler<&ButtonBase::MouseButtonUpEvent>(*this);
		selectedStateChangedEventDispatcher.RemoveHandler<&ButtonBase::SelectedStateChangedEvent>(*this);
		mouseHitStatusChangedEventDispatcher.RemoveHandler<&ButtonBase::HitStatusChangedEvent>(*this);
	}
	void ButtonBase::SetPressedEventCallback(const std::function<void(const PressedEvent&)>& callback)
	{
		this->pressedEventCallback = callback;
	}
	void ButtonBase::SetPressableFlag(bool pressable)
	{
		if (this->pressable != pressable)
		{
			this->pressable = pressable;

			if (!pressable && pressingMouseID.IsValid())
				Unpress(false);
		}
	}
	void ButtonBase::MouseButtonDownEvent(const UIMouseButtonDownEvent& event)
	{
		if (event.button != Input::MouseButton::Left || pressingMouseID.IsValid())
			return;

		if (pressable)
		{
			pressingMouseID = event.mouseID;
			pressedStateChangedEventDispatcher.Call({ *this, pressingMouseID });
			CaptureMouse(event.mouseID);
		}

		Select();
	}
	void ButtonBase::MouseButtonUpEvent(const UIMouseButtonUpEvent& event)
	{
		if (event.button != Input::MouseButton::Left)
			return;

		if (event.mouseID == pressingMouseID)
			Unpress(hovered);
	}
	void ButtonBase::SelectedStateChangedEvent(const InputNode::SelectedStateChangedEvent& event)
	{
		if (!IsSelected())
			Unpress(true);
	}
	void ButtonBase::HitStatusChangedEvent(const UIMouseHitStatusChangedEvent& event)
	{
		hovered = event.newHitStatus != HitStatus::NotHit;
	}
	void ButtonBase::Unpress(bool runPressedEventCallback)
	{
		if (runPressedEventCallback && pressedEventCallback)
			pressedEventCallback({ *this });

		Input::MouseID oldMouseID = pressingMouseID;
		pressingMouseID = Input::MouseID::Invalid();

		ReleaseMouse(oldMouseID);
		pressedStateChangedEventDispatcher.Call({ *this, oldMouseID });
	}
}