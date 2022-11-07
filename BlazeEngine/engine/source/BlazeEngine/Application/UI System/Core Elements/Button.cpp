#include "BlazeEngine/Application/UI System/Core Elements/Button.h"
#include "BlazeEngine/Application/UI System/UIManager.h"
#include "BlazeEngine/Input/Input.h"
#include "BlazeEngine/Application/UI System/UIScene.h"

#include "BlazeEngine/Console/Console.h"

namespace Blaze::UI
{
	void Button::DetachedFromManager()
	{		
		if (state == ButtonState::Down)
		{
			//GetManager()->ReturnFocus();
			GetManager()->AddEventToQueue(released);
			GetManager()->AddEventToQueue(left);
		}
		else if (state == ButtonState::Hovered)
			GetManager()->AddEventToQueue(left);
	}
	Button::Button()
		: state(ButtonState::None)
	{		
		SetProperties({ });
	}
	Button::~Button()
	{
	}

	void Button::SetProperties(const ButtonProperties& p)
	{
		if (p.clickable.edited) clickable = p.clickable;
		if (p.trigerable.edited) triggerable = p.trigerable;
		if (p.pressed.edited) pressed += p.pressed;
		if (p.released .edited) released += p.released;
		if (p.entered.edited) entered += p.entered;
		if (p.left.edited) left += p.left;
	}

	void ButtonManager::Update(UIElement* element)
	{
		auto* manager = GetManager();

		Vec2i mp = Input::GetMousePos() - manager->GetViewport().pos;
		auto mouseState = Input::GetKeyState(MouseKey::MouseLeft);

		if (Input::GetFocusedWindow() == nullptr)
			return;

		Button& button = (Button&)*element;

		ButtonState oldState = button.state;

		if (!button.IsActive() || manager->IsMouseBlocked())
		{
			button.state = UI::ButtonState::None;
		}
		else
		{
			Vec2f pos = button.GetViewportPos();
			Vec2f size = button.GetSize();

			Rectf clipRect = button.GetClipRect();

			if (!isnan(clipRect.x) && !isnan(clipRect.y))
			{
				Vec2f p1 = pos;
				Vec2f p2 = p1 + size;

				Vec2f a1 = clipRect.pos;
				Vec2f a2 = a1 + clipRect.size;

				p1.x = std::clamp(p1.x, a1.x, a2.x);
				p1.y = std::clamp(p1.y, a1.y, a2.y);
				p2.x = std::clamp(p2.x, a1.x, a2.x);
				p2.y = std::clamp(p2.y, a1.y, a2.y);

				pos = p1;
				size = p2 - p1;				
			}

			bool isOver =
				mp.x > pos.x && mp.x < pos.x + size.x &&
				mp.y > pos.y && mp.y < pos.y + size.y;


			if (isOver && (mouseState.pressed && button.state != ButtonState::Down || mouseState.down && button.state == ButtonState::Down) && button.clickable)				
					button.state = ButtonState::Down;
			else if (isOver && button.triggerable)
					button.state = ButtonState::Hovered;
			else
				button.state = ButtonState::None;
		}

		if (oldState == ButtonState::None && button.state != ButtonState::None)
			manager->AddEventToQueue(button.entered);

		if (oldState != ButtonState::Down && button.state == ButtonState::Down)
		{
			manager->AddEventToQueue(button.pressed);
			//Console::WriteLine("Pressed " + String::Convert((size_t)&button));
		}

		if (oldState == ButtonState::Down && button.state != ButtonState::Down)
		{
			manager->AddEventToQueue(button.released);
			//Console::WriteLine("Released " + String::Convert((size_t)&button));		
		}

		if (oldState != ButtonState::None && button.state == ButtonState::None)
			manager->AddEventToQueue(button.left);
	}	

	void ButtonManager::Serialize(UIElement* el, BinaryOutputStream& stream)
	{
		UIBaseElementManager::Serialize(el, stream);
	}
	void ButtonManager::Deserialize(UIElement* el, BinaryInputStream&)
	{

	}	
}