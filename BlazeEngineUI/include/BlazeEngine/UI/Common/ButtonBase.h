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
		};
		struct PressedEvent
		{
			ButtonBase& button;
		};
		EventDispatcher<PressableFlagChangedEvent> pressableFlagChangedEventDispatcher;
		EventDispatcher<PressedStateChangedEvent> pressedStateChangedEventDispatcher;

		ButtonBase();
		~ButtonBase();

		int HitTest(Vec2f screenPos) override;

		void SetPressedEventCallback(const std::function<void(const PressedEvent&)>& callback);
		void SetPressableFlag(bool pressable);
		/*
			Unpresses the node

			\param quiet   if 'quiet' is true and the node is pressed no PressedEvent event will be dispatched,
			otherwise if 'quiet' is false the node is pressed a PressedEvent will be dispatcher
		*/
		void Unpress(bool quiet);
		/*
			"Presses" the node, basically simulates the node being pressed. Calls the PressedEvent callback
		*/
		void Press();

		inline bool IsPressed() const { return pressed; }
		inline bool IsPressable() const { return pressable; }
	protected:
		void MouseButtonDownEvent(const MouseButtonDownEvent& event);
		void MouseButtonUpEvent(const MouseButtonUpEvent& event);
		void SelectedStateChangedEvent(const SelectedStateChangedEvent& event);
	private:
		bool pressed : 1;
		bool pressable : 1;
		std::function<void(const PressedEvent&)> pressedEventCallback;
	};
}