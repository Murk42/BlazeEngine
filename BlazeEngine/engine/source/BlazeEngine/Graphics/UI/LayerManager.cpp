#include "BlazeEngine/Graphics/UI/LayerManager.h"
#include "BlazeEngine/Graphics/UI/Layer.h"

#include "BlazeEngine/Graphics/Renderer.h"
#include "BlazeEngine/Math/Math.h"

namespace Blaze::UI
{	
	void LayerManager::AddLayer(Layer& layer)
	{		
		layer.manager = this;
		layers.emplace_back(&layer);
	}

	void LayerManager::SetupLayers()
	{
		for (auto& layer : layers)
		{						
			layer->textRenderer.Load();
			layer->panelRenderer.Load();
			layer->Setup();
		}
	}

	void LayerManager::UpdateLayers()
	{
		for (auto& layer : layers)
			if (layer->active)
				layer->Update();
	}

	void LayerManager::RenderLayers()
	{
		for (auto& layer : layers)
			if (layer->active)
			{
				layer->panelRenderer.Render(proj);
				layer->textRenderer.Render(proj);
				Renderer::ClearTargetDepth();
			}
	}

	void LayerManager::SetViewportSize(Vec2i size)
	{
		proj = Math::OrthographicMatrix<float>(0, size.x, 0, size.y, -1, 1);
		viewportSize = size;

		for (auto& layer : layers)
			if (layer->active)
			{
				layer->NewViewport(size);
			}
	}

	void LayerManager::MouseMotionEvent(InputEvent::MouseMotion event)
	{
		for (auto& layer : layers)
			if (layer->active && layer->interactable)
			{				
				layer->buttonManager.MouseMotionEvent(event);
				layer->textFieldManager.MouseMotionEvent(event);
			}
	}

	void LayerManager::MousePressedEvent(InputEvent::MousePressed event)
	{
		for (auto& layer : layers)
			if (layer->active && layer->interactable)
			{
				layer->stateManager.MousePressedEvent(event);
				layer->buttonManager.MousePressedEvent(event);
				layer->textFieldManager.MousePressedEvent(event);
			}
	}

	void LayerManager::MouseReleasedEvent(InputEvent::MouseReleased event)
	{
		for (auto& layer : layers)
			if (layer->active && layer->interactable)
			{				
				layer->buttonManager.MouseReleasedEvent(event);
				layer->textFieldManager.MouseReleasedEvent(event);
			}
	}

	void LayerManager::KeyPressedEvent(InputEvent::KeyPressed event)
	{
		for (auto& layer : layers)
			if (layer->active && layer->interactable)
			{
				layer->textFieldManager.KeyPressedEvent(event);
			}
	}

	void LayerManager::TextInputEvent(InputEvent::TextInput event)
	{
		for (auto& layer : layers)
			if (layer->active && layer->interactable)
			{
				layer->textFieldManager.TextInputEvent(event);
			}
	}
}