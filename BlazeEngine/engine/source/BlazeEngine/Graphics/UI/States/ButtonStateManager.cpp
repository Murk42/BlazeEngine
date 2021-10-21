#include "BlazeEngine/Graphics/UI/States/ButtonStateManager.h"
#include "BlazeEngine/Graphics/UI/States/ButtonState.h"
#include "BlazeEngine/Graphics/UI/Elements/Element.h"

namespace Blaze::UI
{
	bool IsInside(Vec2f pos, Vec2f size)
	{
		constexpr float radius = 5;
		float x = std::max(std::abs(pos.x - size.x / 2) - size.x / 2 + radius, 0.0f) / radius;
		float y = std::max(std::abs(pos.y - size.y / 2) - size.y / 2 + radius, 0.0f) / radius;
		return x * x + y * y < 1.0f;
	}

	void ButtonStateManager::AddButton(ButtonState& button)
	{
		buttons.emplace_back(&button);
	}

	void ButtonStateManager::MouseMotionEvent(InputEvent::MouseMotion event)
	{
		for (auto& button : buttons)
		{
			bool over = IsInside(event.pos - button->parent->transform.absolutePos, button->parent->transform.absoluteSize);

			if (button->hovered != over)
			{
				button->hovered = over;
				if (button->StateChangedEvent)
					button->StateChangedEvent();
			}
			button->hovered = over;
		}
	}

	void ButtonStateManager::MousePressedEvent(InputEvent::MousePressed event)
	{
		for (auto& button : buttons)
		{
			button->down = IsInside(event.pos - button->parent->transform.absolutePos, button->parent->transform.absoluteSize);

			if (button->down)
			{
				if (button->StateChangedEvent)
					button->StateChangedEvent();
				if (button->PressedEvent)
					button->PressedEvent();
			}
		}
	}

	void ButtonStateManager::MouseReleasedEvent(InputEvent::MouseReleased event)
	{
		for (auto& button : buttons)
		{
			if (button->down)
			{
				button->down = false;
				if (button->StateChangedEvent)
					button->StateChangedEvent();
				if (button->ReleasedEvent)
					button->ReleasedEvent();
			}
		}
	}
}