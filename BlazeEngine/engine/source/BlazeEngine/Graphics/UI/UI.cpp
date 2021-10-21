#include "BlazeEngine/Graphics/UI/UI.h"
#include "BlazeEngine/Graphics/UI/Layer.h"
#include "BlazeEngine/Graphics/UI/Elements/Element.h"

#include "BlazeEngine/Graphics/Renderer.h"
#include "BlazeEngine/Math/Math.h"

#include "source/BlazeEngine/Internal/Engine.h"

namespace Blaze
{
	namespace UI
	{
		void RenderUI()
		{
			engine->UI_Render();
		}
	}

	void Engine::UI_Render()
	{
		for (auto& layer : engine->UI.layers)
			if (layer->active)
			{
				layer->panelRenderer.Render(UI.proj);
				layer->textRenderer.Render(UI.proj);
				Renderer::ClearTargetDepth();
			}
	}

	void Engine::UI_Setup()
	{		
		for (auto& layer : UI.layers)
		{			
			layer->textRenderer.Load();
			layer->panelRenderer.Load();
			layer->Setup();

			if (UI.viewportSize.x != 0)
				layer->NewViewport(UI.viewportSize);
		}
	}
	void Engine::UI_MouseMotionEvent(InputEvent::MouseMotion event)
	{
		for (auto& layer : UI.layers)
			if (layer->active && layer->interactable)
			{
				layer->buttonManager.MouseMotionEvent(event);
				layer->textFieldManager.MouseMotionEvent(event);
			}
	}
	void Engine::UI_MousePressedEvent(InputEvent::MousePressed event)
	{
		for (auto& layer : UI.layers)
			if (layer->active && layer->interactable)
			{
				layer->stateManager.MousePressedEvent(event);
				layer->buttonManager.MousePressedEvent(event);
				layer->textFieldManager.MousePressedEvent(event);
			}
	}
	void Engine::UI_MouseReleasedEvent(InputEvent::MouseReleased event)
	{
		for (auto& layer : UI.layers)
			if (layer->active && layer->interactable)
			{
				layer->buttonManager.MouseReleasedEvent(event);
				layer->textFieldManager.MouseReleasedEvent(event);
			}
	}
	void Engine::UI_KeyPressedEvent(InputEvent::KeyPressed event)
	{
		for (auto& layer : UI.layers)
			if (layer->active && layer->interactable)
			{
				layer->textFieldManager.KeyPressedEvent(event);
			}
	}
	void Engine::UI_TextInputEvent(InputEvent::TextInput event)
	{
		for (auto& layer : UI.layers)
			if (layer->active && layer->interactable)
			{
				layer->textFieldManager.TextInputEvent(event);
			}
	}
	void Engine::UI_SetViewportSize(Vec2i size)
	{
		UI.proj = Math::OrthographicMatrix<float>(0, size.x, 0, size.y, -1, 1);
		UI.viewportSize = size;

		for (auto& layer : UI.layers)
		{
			layer->NewViewport(size);
			for (auto& element : layer->elementManager.elements)
				element->UpdateTransform();
		}
	}
}		