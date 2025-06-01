#pragma once
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineCore/Event/EventDispatcher.h"
#include "BlazeEngineGraphics/UI/Input/InputNode.h"
#include <functional>

namespace Blaze::UI
{
	template<typename C, intMem offset, auto FP, auto DP>
	class MemberFunctionEventDispatcherBind
	{
	public:
		MemberFunctionEventDispatcherBind()
		{
			C& object = *((C*)((byte*)this + offset));
			object.DP.Call<FP>(object);
		}
		~MemberFunctionEventDispatcherBind()
		{
			C& object = *((C*)((byte*)this + offset));
			object.DP.Call<FP>(object);
		}
	};

#define BIND_EVENT_FUNCTION(className, functionName, dispatcherName) BLAZE_ALLOCATOR_ATTRIBUTE MemberFunctionEventDispatcherBind<className, offsetof(className, dispatcherName), &className::functionName, &className::dispatcherName> functionName ## Binding

	class BLAZE_GRAPHICS_API ButtonBase :
		public InputNode
	{
	public:
		struct PressableFlagChangedEvent
		{
			ButtonBase& button;
		};
		struct PressedEvent 
		{
			ButtonBase& button;
		};
		EventDispatcher<PressableFlagChangedEvent> pressableFlagChangedEventDispatcher;
		std::function<void(const PressedEvent&)> pressedEventCallback;

		ButtonBase();
		~ButtonBase();

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

		//BIND_EVENT_FUNCTION(ButtonBase, MouseButtonDownEvent, mouseButtonDownEventDispatcher);
	};
}