#include "pch.h"
#include "BlazeEngine/Runtime/IO/Input.h"
#include "BlazeEngine/UI/Common/ButtonBase.h"
#include "BlazeEngine/UI/Input/InputSubSystem.h"

namespace Blaze::UI
{
	ButtonBase::ButtonBase()
		: pressingMouseID(UINT32_MAX), pressable(true)
	{
		mouseButtonDownEventDispatcher.AddHandler<&ButtonBase::MouseButtonDownEvent>(*this);
		mouseButtonUpEventDispatcher.AddHandler<&ButtonBase::MouseButtonUpEvent>(*this);
		selectedStateChangedEventDispatcher.AddHandler<&ButtonBase::SelectedStateChangedEvent>(*this);
		mouseHitStatusChangedEventDispatcher.AddHandler<&ButtonBase::HitStatusChangedEvent>(*this);
	}
	ButtonBase::ButtonBase(Node& parent, const NodeTransform& nodeTransform)
		: ButtonBase()
	{
		SetParent(&parent);
		SetTransform(nodeTransform);
	}
	ButtonBase::~ButtonBase()
	{
		mouseButtonDownEventDispatcher.RemoveHandler<&ButtonBase::MouseButtonDownEvent>(*this);
		mouseButtonUpEventDispatcher.RemoveHandler<&ButtonBase::MouseButtonUpEvent>(*this);
		selectedStateChangedEventDispatcher.RemoveHandler<&ButtonBase::SelectedStateChangedEvent>(*this);
		mouseHitStatusChangedEventDispatcher.RemoveHandler<&ButtonBase::HitStatusChangedEvent>(*this);
	}
	void ButtonBase::SetPressableFlag(bool pressable)
	{
		if (this->pressable != pressable)
		{
			this->pressable = pressable;

			if (!pressable && pressingMouseID.IsValid())
				Unpress(false, Vec2f(0, 0), false);
		}
	}
	void ButtonBase::MouseButtonDownEvent(const UIMouseButtonDownEvent& event)
	{
		if (event.button != Input::MouseButton::Left || pressingMouseID.IsValid())
			return;

		if (pressable)
		{
			pressingMouseID = event.mouseID;
			IsDownChanged(true, event.pos);
			CaptureMouse(event.mouseID);
		}

		Select();
	}
	void ButtonBase::MouseButtonUpEvent(const UIMouseButtonUpEvent& event)
	{
		if (event.button != Input::MouseButton::Left)
			return;

		if (event.mouseID == pressingMouseID)
			Unpress(true, event.pos, mouseOver);
	}
	void ButtonBase::SelectedStateChangedEvent(const InputNode::SelectedStateChangedEvent& event)
	{
		if (!IsSelected())
			Unpress(false, Vec2f(0, 0), false);
	}
	void ButtonBase::HitStatusChangedEvent(const UIMouseHitStatusChangedEvent& event)
	{
		bool oldHovered = mouseOver;
		mouseOver = event.newHitStatus != HitStatus::NotHit;

		if (mouseOver != oldHovered)
			IsMouseOverChanged(event.pos);
	}
	void ButtonBase::Unpress(bool byPointer, Vec2f pos, bool runPressedEventCallback)
	{
		if (runPressedEventCallback)
			Pressed();

		Input::MouseID oldMouseID = pressingMouseID;
		pressingMouseID = Input::MouseID::Invalid();
		
		ReleaseMouse(oldMouseID);
		IsDownChanged(byPointer, pos);
	}
}