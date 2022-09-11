#include "BlazeEngine/Application/UI System/Core Elements/Button.h"
#include "BlazeEngine/Application/UI System/UIManager.h"
#include "BlazeEngine/Input/Input.h"
#include "BlazeEngine/Application/UI System/UIScene.h"

namespace Blaze::UI
{
	Button::Button()
		: state(UI::ButtonState::None)
	{
		SetClickableFlag(true);
	}
	Button::~Button()
	{
	}


	void ButtonManager::Update(size_t index, size_t end)
	{		
		size_t front = index - 1;
		index = end - 1;
		auto mouseState = Input::GetKeyState(MouseKey::MouseLeft);
		Vec2f mp = (Vec2f)Input::GetMousePos();
		for (; index != front; --index)
		{
			Button& button = *GetElement(index);
			if (!button.IsActive())
			{ 
				UI::ButtonState old = button.state;
				button.state = UI::ButtonState::None;
				if (old == UI::ButtonState::Down)
				{
					button.released();
					button.left();
				}
				else if (old == UI::ButtonState::Hovered)
					button.left();
				continue;
			}

			Rectf alignedRect = button.GetAlignedRect();

			bool isOver =
				mp.x > alignedRect.x && mp.x < alignedRect.x + alignedRect.w &&
				mp.y > alignedRect.y && mp.y < alignedRect.y + alignedRect.h;			

			bool callPressed = false;
			bool callReleased = false;
			bool callEntered = false;
			bool callLeft = false;

			if (isOver && mouseState.pressed == true && !GetManager()->IsFocusTaken() && button.IsClickable())
				callPressed = true;
			if (mouseState.released == true && button.state == UI::ButtonState::Down)
				callReleased = true;
			if (isOver && button.state == UI::ButtonState::None)
				callEntered = true;
			if (!isOver && button.state == UI::ButtonState::Hovered || !isOver && callReleased)
				callLeft = true;

			if (callPressed)
				button.state = UI::ButtonState::Down;
			if (callReleased && isOver || callEntered)
				button.state = UI::ButtonState::Hovered;
			if (callLeft)
				button.state = UI::ButtonState::None;

			if (callPressed)
				GetManager()->TakeFocus();
			if (callReleased)
				GetManager()->ReturnFocus();

			if (callEntered) button.entered();
			if (callPressed) button.pressed();
			if (callReleased) button.released();
			if (callLeft) button.left();
		}		
	}
	UIElementParsingData ButtonManager::GetElementParsingData()
	{
		UIElementParsingData data = UIBaseElementManager::GetElementParsingData();		
		data.AddMember<Button, String>("pressed", [](UIScene& scene, Button& el, const String& name) {
			el.pressed += scene.GetEventFunction(name);
			}, nullptr);
		data.AddMember<Button, String>("released", [](UIScene& scene, Button& el, const String& name) {
			el.released += scene.GetEventFunction(name);
			}, nullptr);
		data.AddMember<Button, String>("entered", [](UIScene& scene, Button& el, const String& name) {
			el.entered += scene.GetEventFunction(name);
			}, nullptr);
		data.AddMember<Button, String>("left", [](UIScene& scene, Button& el, const String& name) {
			el.left += scene.GetEventFunction(name);
			}, nullptr);
		return data;
	}
}