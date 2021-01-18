#include "BlazeEngine/Graphics/UI/Button.h"
#include "BlazeEngine/Input/Input.h"
#include "BlazeEngine/Core/Window.h"

namespace Blaze
{
	inline void CallEventFunction(void* function, Button* button)
	{
		if (function != nullptr)
			((void(*)(Button*))function)(button);
	}

	Button::Button()
		: state(ButtonState::Up)
	{
		mesh.SetVertices(&vertex, 1);
	}
	Button::Button(Recti textureCenterRect)
		: textureCenterRect(textureCenterRect)
	{

	}

	void Button::SetEventFunction(ButtonEvent event, void* function)
	{
		eventFunctions[(uint)event] = function;
	}

	void Button::Update()
	{
		Window* focusedWindow = Input::GetFocusedWindow();
		Vec2i mousePos = Input::GetMousePos();

		if (focusedWindow != nullptr)
		{			
			mousePos.y = focusedWindow->GetSize().y - mousePos.y;

			mousePos -= transform.absolutePos;

			if (mousePos.x > 0 && mousePos.y > 0 && mousePos.x < transform.size.x && mousePos.y < transform.size.y)
			{
				KeyState keyState = Input::GetKeyState(Key::MouseLeft);
				
				if (state == ButtonState::Pressed)
					state = ButtonState::Down;

				if (keyState == KeyState::Pressed)
				{
					if (state == ButtonState::Up || state == ButtonState::Released || state == ButtonState::Hovered)
						state = ButtonState::Pressed;
				}				
				else if (keyState != KeyState::Down)
					state = ButtonState::Hovered;
			}
			else
			{			
				if (state == ButtonState::Released)
					state = ButtonState::Up;
				if (state == ButtonState::Down || state == ButtonState::Released || state == ButtonState::Hovered)
					state = ButtonState::Released;
			}
		}

		switch (state)
		{
		case ButtonState::Down:
			CallEventFunction(eventFunctions[(uint)ButtonEvent::Down], this);
			break;
		case ButtonState::Hovered:
		case ButtonState::Up:
			CallEventFunction(eventFunctions[(uint)ButtonEvent::Up], this);
			break;
		case ButtonState::Pressed:
			CallEventFunction(eventFunctions[(uint)ButtonEvent::Pressed], this);
			break;
		case ButtonState::Released:
			CallEventFunction(eventFunctions[(uint)ButtonEvent::Released], this);
			break;
		}

		vertex.GetValue<0>() = transform.size;
		vertex.GetValue<1>() = Vec4f(textureCenterRect.x, textureCenterRect.y, textureCenterRect.w, textureCenterRect.h);
		vertex.GetValue<3>() = transform.depth;
		switch (state)
		{
		case ButtonState::Released:
		case ButtonState::Up:
			vertex.GetValue<2>() = 2.0f;
			break;
		case ButtonState::Hovered:
			vertex.GetValue<2>() = 1.0f;
			break;
		case ButtonState::Pressed:
		case ButtonState::Down:
			vertex.GetValue<2>() = 0.0f;
			break;
		}

		mesh.ChangeVertices(&vertex, 1, 0);
	}	
}