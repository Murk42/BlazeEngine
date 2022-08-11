#include "BlazeEngine/Application/UI System/Core Elements/Button.h"
#include "BlazeEngine/Application/UI System/UIManager.h"
#include "BlazeEngine/Input/Input.h"


namespace Blaze::UI
{
	Button::Button()
		: active(true), state(UI::ButtonState::None)
	{
	}

	void ButtonManager::Update(size_t index, size_t end)
	{		
		auto mouseState = Input::GetKeyState(MouseKey::MouseLeft);
		Vec2f mp = (Vec2f)Input::GetMousePos();
		for (; index != end; ++index)
		{
			Button& button = *GetElement(index);
			if (!button.active)
			{
				button.state = UI::ButtonState::None;
				if (button.state == UI::ButtonState::Down)
				{
					button.released();
					button.left();
				}
				else if (button.state == UI::ButtonState::Hovered)
					button.left();
				return;
			}

			Rectf alignedRect = button.GetAlignedRect();

			bool isOver =
				mp.x > alignedRect.x && mp.x < alignedRect.x + alignedRect.w &&
				mp.y > alignedRect.y && mp.y < alignedRect.y + alignedRect.h;			

			bool callPressed = false;
			bool callReleased = false;
			bool callEntered = false;
			bool callLeft = false;

			if (isOver && mouseState.pressed == true && !GetManager()->IsFocusTaken())
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

			if (callEntered && button.entered)
				button.entered();
			if (callPressed && button.pressed)
				button.pressed();
			if (callReleased && button.released)
				button.released();
			if (callLeft && button.left)
				button.left();
		}		
	}
}