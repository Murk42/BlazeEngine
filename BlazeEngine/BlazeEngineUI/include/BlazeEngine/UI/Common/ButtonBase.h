#pragma once
#include "BlazeEngine/Core/Event/EventDispatcher.h"
#include "BlazeEngine/UI/Input/InputNode.h"
#include <functional>

namespace Blaze::UI
{
	class BLAZE_API ButtonBase :
		public InputNode
	{
	public:
		struct PressableFlagChangedEvent
		{
			ButtonBase& button;
		};
		struct PressedStateChangedEvent
		{
			ButtonBase& button;
			Input::MouseID mouseID;
		};
		struct PressedEvent
		{
			ButtonBase& button;
		};
		EventDispatcher<PressableFlagChangedEvent> pressableFlagChangedEventDispatcher;
		EventDispatcher<PressedStateChangedEvent> pressedStateChangedEventDispatcher;

		ButtonBase();
		~ButtonBase();

		void SetPressedEventCallback(const std::function<void(const PressedEvent&)>& callback);
		void SetPressableFlag(bool pressable);

		inline bool IsPressed() const { return pressingMouseID.IsValid(); }
		inline Input::MouseID GetPressingMouseID() const { return pressingMouseID; }
		inline bool IsPressable() const { return pressable; }
	protected:
		void MouseButtonDownEvent(const UIMouseButtonDownEvent& event);
		void MouseButtonUpEvent(const UIMouseButtonUpEvent& event);
		void SelectedStateChangedEvent(const SelectedStateChangedEvent& event);
		void HitStatusChangedEvent(const UIMouseHitStatusChangedEvent& event);
	private:
		bool pressable : 1;
		bool hovered : 1;
		Input::MouseID pressingMouseID;
		std::function<void(const PressedEvent&)> pressedEventCallback;

		void Unpress(bool runPressedEventCallback);
	};
}