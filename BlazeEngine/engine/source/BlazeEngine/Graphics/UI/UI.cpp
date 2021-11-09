#include "BlazeEngine/Graphics/UI/UI.h"
#include "BlazeEngine/Graphics/UI/Layer.h"
#include "BlazeEngine/Graphics/UI/Elements/Element.h"
#include "BlazeEngine/Graphics/UI/Elements/Panel.h"
#include "BlazeEngine/Graphics/UI/Elements/Text.h"

#include "BlazeEngine/Graphics/Renderer.h"
#include "BlazeEngine/Math/Math.h"

#include "source/BlazeEngine/Internal/Engine.h"

namespace Blaze
{
	namespace UI
	{
		void RenderUI()
		{			
		}
	}

	struct PanelVertex
	{
		Mat4f mat;
		Vec2f size;
		Vec4f color;
		Vec4f outlineColor;
		float radius;
		float thickness;
	};
	constexpr size_t vertexCount = 128;
	PanelVertex panelVertices[vertexCount];	
	
	void UI_SetupPanels(UI::Layer* layer)
	{
		/*
		UI::Layer::PanelData& data = *layer->panelData;
		uint i = 0;
		while (i < layer->panel.panels.size())
		{
			uint j = 0;
			while (j < vertexCount && i < layer->panel.panels.size());
			{
				panelVertices[j].mat = layer->panel.panels[i]->transform.mat;
				panelVertices[j].color = layer->panel.panels[i]->color;
				panelVertices[j].size = layer->panel.panels[i]->transform.absoluteSize;
				panelVertices[j].outlineColor = layer->panel.panels[i]->outlineColor;
				panelVertices[j].radius = layer->panel.panels[i]->radius;
				panelVertices[j].thickness = layer->panel.panels[i]->thickness;
				++i;
				++j;
			}

			layer->panel.vb.AllocateDynamicStorage(BufferView(panelVertices, j * sizeof(PanelVertex)), Graphics::Core::GraphicsBufferDynamicStorageHint::DynamicDraw);

			Renderer::SelectVertexArray(&layer->panel.va);
			Renderer::SelectProgram(&layer->panel.program);
			layer->panel.program.SetUniform(layer->panel.uniformVP, layer->panel.uniformVP);

			Renderer::RenderPointArray(j, 0);
		}*/
	}

	void UI_SetupTexts(UI::Layer*)
	{
	}

	void UI_RenderPanels(UI::Layer*)
	{
	}

	void UI_RenderTexts(UI::Layer*)
	{
	}

	void UI_Render()
	{
		/*
		for (auto& layer : engine->UI.layers)
			if (layer->active)
			{
				
				
				layer->textRenderer.Render(UI.proj);
				Renderer::ClearTargetDepth();
			}*/
	}

	void UI_Setup()
	{		/*
		for (auto& layer : UI.layers)
		{			
			UI.currentLayer = layer;
			layer->textRenderer.Load();
			layer->panelRenderer.Load();
			layer->Setup();

			if (UI.viewportSize.x != 0)
				layer->NewViewport(UI.viewportSize);
		}*/
	}
	void UI_MouseMotionEvent(InputEvent::MouseMotion event)
	{/*
		for (auto& layer : UI.layers)
			if (layer->active && layer->interactable)
			{
				layer->buttonManager.MouseMotionEvent(event);
				layer->textFieldManager.MouseMotionEvent(event);
			}*/
	}
	void UI_MousePressedEvent(InputEvent::MousePressed event)
	{/*
		for (auto& layer : UI.layers)
			if (layer->active && layer->interactable)
			{
				layer->stateManager.MousePressedEvent(event);
				layer->buttonManager.MousePressedEvent(event);
				layer->textFieldManager.MousePressedEvent(event);
			}*/
	}
	void UI_MouseReleasedEvent(InputEvent::MouseReleased event)
	{/*
		for (auto& layer : UI.layers)
			if (layer->active && layer->interactable)
			{
				layer->buttonManager.MouseReleasedEvent(event);
				layer->textFieldManager.MouseReleasedEvent(event);
			}*/
	}
	void UI_KeyPressedEvent(InputEvent::KeyPressed event)
	{/*
		for (auto& layer : UI.layers)
			if (layer->active && layer->interactable)
			{
				layer->textFieldManager.KeyPressedEvent(event);
			}*/
	}
	void UI_TextInputEvent(InputEvent::TextInput event)
	{/*
		for (auto& layer : UI.layers)
			if (layer->active && layer->interactable)
			{
				layer->textFieldManager.TextInputEvent(event);
			}*/
	}
	void UI_SetViewportSize(Vec2i size)
	{/*
		UI.proj = Math::OrthographicMatrix<float>(0, size.x, 0, size.y, -1, 1);
		UI.viewportSize = size;

		for (auto& layer : UI.layers)
		{
			layer->NewViewport(size);
			for (auto& element : layer->elementManager.elements)
				element->UpdateTransform();
		}*/
	}
}		