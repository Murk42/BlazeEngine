#include "BlazeEngine/Graphics/UI/Button.h"
#include "BlazeEngine/Input/Input.h"
#include "BlazeEngine/Core/Window.h"
#include "BlazeEngine/Graphics/Renderer.h"

namespace Blaze
{
	inline void CallEventFunction(std::function<void(Button*)>& func, Button* button)
	{		
		if (func)
			func(button);
	}

	Button::Button()
		: state(ButtonState::Up)
	{
		mesh.SetVertices(&vertex, 1);
	}
	Button::Button(ButtonData* buttonData)
		: state(ButtonState::Up), buttonData(buttonData)
	{
	}

	void Button::SetEventFunction(ButtonEvent event, void* function)
	{		
		switch (event)
		{
		case Blaze::ButtonEvent::Down:			
		case Blaze::ButtonEvent::Up:			
		case Blaze::ButtonEvent::Pressed:			
		case Blaze::ButtonEvent::Released:			
		eventFunctions[(uint)event].func = *(std::function<void(Button*)>*)function;
		break;
		default:
			break;
		}
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
				KeyState keyState = Input::GetKeyState(Key::MouseLeft).state;
				
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
			CallEventFunction(eventFunctions[(uint)ButtonEvent::Down].func, this);
			break;
		case ButtonState::Hovered:
		case ButtonState::Up:
			CallEventFunction(eventFunctions[(uint)ButtonEvent::Up].func, this);
			break;
		case ButtonState::Pressed:
			CallEventFunction(eventFunctions[(uint)ButtonEvent::Pressed].func, this);
			break;
		case ButtonState::Released:
			CallEventFunction(eventFunctions[(uint)ButtonEvent::Released].func, this);
			break;
		}

		vertex.GetValue<0>() = transform.size;		
		vertex.GetValue<2>() = transform.depth;

		switch (state)
		{
		case ButtonState::Released:
		case ButtonState::Up:
			vertex.GetValue<1>() = 2.0f;
			break;
		case ButtonState::Hovered:
			vertex.GetValue<1>() = 1.0f;
			break;
		case ButtonState::Pressed:
		case ButtonState::Down:
			vertex.GetValue<1>() = 0.0f;
			break;
		}

		mesh.ChangeVertices(&vertex, 1, 0); 
	}
	void Button::Render(BaseMaterial& material, const Mat4f& mat)
	{		
		BaseMaterialProperties::Property<Vec4f>* textureCenterRect = material.GetProperty<Vec4f>("u_textureCenterRect");
		BaseMaterialProperties::Property<float>* texScale = material.GetProperty<float>("u_texScale");

		if (material.standardProperties.MVP == nullptr || textureCenterRect == nullptr || texScale == nullptr)
		{
			Logger::AddLog(LogType::Error, __FUNCTION__, "Button::Render requires material with MVP, textureCenterRect, texture and texScale property.");
			return;
		}
		if (buttonData == nullptr)
		{
			Logger::AddLog(LogType::Error, __FUNCTION__, "No buttonData set");
			return;
		}

		*textureCenterRect = Vec4f(buttonData->textureCenterRect.x, buttonData->textureCenterRect.y, buttonData->textureCenterRect.w, buttonData->textureCenterRect.h);
		*texScale = buttonData->texScale;
		*material.standardProperties.texture = buttonData->texture;
		*material.standardProperties.MVP = mat;
		Renderer::RenderPointArray(material, mesh);
	}
}