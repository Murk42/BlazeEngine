#pragma once
#include "BlazeEngine/UI/Input/InputNode.h"

namespace Blaze::UI
{
	class BLAZE_API ButtonBase : public InputNode
	{
	public:
		ButtonBase();
		ButtonBase(Node& parent, const NodeTransform& nodeTransform);
		~ButtonBase();

		void SetPressableFlag(bool pressable);

		inline bool IsDown() const { return pressingMouseID.IsValid(); }
		inline bool IsMouseOver() const { return mouseOver; }
		inline bool IsPressable() const { return pressable; }
		inline Input::MouseID GetPressingMouseID() const { return pressingMouseID; }
	protected:
		void MouseButtonDownEvent(const UIMouseButtonDownEvent& event);
		void MouseButtonUpEvent(const UIMouseButtonUpEvent& event);
		void SelectedStateChangedEvent(const SelectedStateChangedEvent& event);
		void HitStatusChangedEvent(const UIMouseHitStatusChangedEvent& event);
		
		/*
			This function is called only when the button is intentionally pressed, and should be used as the 
			high level result. 
			
			This means, while the button is down and the mouse is hovering over the button, then if the left 
			click is released the button is pressed.

			For example, if the button is down but the mouse isn't hovering over the button, then if the left 
			click is released the button isn't considered to be pressed. Or if the button is down and the mouse 
			is hovering over the button but the button has been disabled, unselected or the pressable flag has 
			been set to false before the left click is released, the button isn't considered to be pressed.
		*/
		virtual void Pressed() { }
		/*
			This function is called when the button changes it's down state, regardless if it was done by the 
			user intentionally or not. 
			
			It can be used to update the visual state of the button.
		*/
		virtual void IsDownChanged(bool byPointer, Vec2f pos) { }
		/*
			This function is called when the mouse enters or leaves the button.

			It can be used to update the visual state of the button.
		*/
		virtual void IsMouseOverChanged(Vec2f pos) { }
		virtual void PressableFlagChanged() { }
	private:
		bool pressable : 1;
		bool mouseOver : 1;
		Input::MouseID pressingMouseID; 

		void Unpress(bool byPointer, Vec2f pos, bool runPressedEventCallback);
	};
}