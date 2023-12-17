#include "pch.h"
#include "UIRenderPipeline_OpenGL.h"

namespace Blaze::Graphics::OpenGL
{
	UIRenderPipeline_OpenGL::UIRenderPipeline_OpenGL(TexturedRectRenderer_OpenGL& texturedRectRenderer, PanelRenderer_OpenGL& panelRenderer, TextRenderer_OpenGL& textRenderer) :
		texturedRectRenderer(texturedRectRenderer),
		panelRenderer(panelRenderer),
		textRenderer(textRenderer)
	{
	}

	UIRenderPipeline_OpenGL::~UIRenderPipeline_OpenGL()
	{
		for (auto& layer : layers)
		{
			for (auto& node : layer.panels)
				UnsubscribeNode(node);
			for (auto& node : layer.images)
				UnsubscribeNode(node);
			for (auto& node : layer.texts)
				UnsubscribeNode(node);
		}
	}

	void UIRenderPipeline_OpenGL::SetScreen(UI2::Screen* newScreen)
	{
		if (screen != nullptr)
		{
			screen->nodeCreatedEventDispatcher.RemoveHandler(*this);
			screen->nodeDestroyedEventDispatcher.RemoveHandler(*this);		
			screen->screenDestructionEventDispatcher.RemoveHandler(*this);
		}

		screen = newScreen;

		if (screen != nullptr)
		{
			screen->nodeCreatedEventDispatcher.AddHandler(*this);
			screen->nodeDestroyedEventDispatcher.AddHandler(*this);
			screen->screenDestructionEventDispatcher.AddHandler(*this);

			for (auto& child : newScreen->GetChildren())
				AddNodeTree(child);
		}
		else
		{
			layers.Clear();
		}
	}

	void UIRenderPipeline_OpenGL::Render(Vec2u targetSize)
	{
		if (screen == nullptr)
			return;		

		for (auto& layer : layers)
		{
			if (layer.panelsDirty) RecreatePanelRenderData(layer);							
			panelRenderer.Render(layer.panelRenderCache, targetSize);
			if (layer.imagesDirty) RecreateImageRenderData(layer);
			texturedRectRenderer.Render(layer.texturedRectRenderCache, targetSize);
			if (layer.textsDirty) RecreateTextRenderData(layer);
			textRenderer.Render(layer.textRenderCache, targetSize);
		}
	}

	void UIRenderPipeline_OpenGL::RecreatePanelRenderData(LayerData& layer)
	{
		Array<PanelRenderData_OpenGL> renderData{ layer.panels.Count() };

		for (uint i = 0; i < layer.panels.Count(); ++i)
		{
			auto& rd = renderData[i];
			auto& panel = layer.panels[i];

			rd.fillColor = panel->fillColor;
			rd.borderColor = panel->borderColor;
			rd.cornerRadius = panel->cornerRadius;
			rd.borderWidth = panel->borderWidth;
			rd.pos = panel->GetFinalPosition();
			rd.size = panel->GetFinalSize();
			rd.rotation = panel->GetFinalRotation();			
		}

		layer.panelRenderCache.CreateNew(panelRenderer, renderData);
		layer.panelsDirty = false;
	}

	void UIRenderPipeline_OpenGL::RecreateImageRenderData(LayerData& layer)
	{
		Array<TexturedRectRenderData_OpenGL> renderData{ layer.images.Count() };

		for (uint i = 0; i < layer.images.Count(); ++i)
		{
			auto& rd = renderData[i];
			auto& image = layer.images[i];

			rd.texture = image->texture;
			rd.uv1 = image->uv1;
			rd.uv2 = image->uv2;
			rd.color = image->color;
			rd.blend = image->blend;
			rd.alpha = image->alpha;

			float rotation = image->GetFinalRotation();
			Vec2f right = { Math::Cos(rotation), Math::Sin(rotation) };
			Vec2f size = image->GetFinalSize();

			rd.pos = image->GetFinalPosition();
			rd.right = right * size.x;
			rd.up = Vec2f(-right.y, right.x) * size.y;
		}

		layer.texturedRectRenderCache.CreateNew(texturedRectRenderer, renderData);
		layer.imagesDirty = false;
	}

	void UIRenderPipeline_OpenGL::RecreateTextRenderData(LayerData& layer)
	{
		Array<TextRenderData_OpenGL> renderData{ layer.texts.Count() };
		SingleLineTextLayouter layouter;

		for (uint i = 0; i < layer.texts.Count(); ++i)
		{
			auto& rd = renderData[i];
			auto& text = layer.texts[i];
			
			rd.font = text->GetFont();
			rd.atlasFontHeight = text->GetPixelFontHeight();
			rd.pos = text->GetFinalPosition();
			rd.rotation = text->GetFinalRotation();			
			rd.characterData.ReserveExactly(text->GetCharacterData().Count());			

			for (auto& data1 : text->GetCharacterData())
			{
				auto& data2 = *rd.characterData.AddBack();
				data2.character = data1.character;
				data2.color = data1.color;
				data2.pos = (Vec2f)data1.pos;
				data2.size = (Vec2f)data1.size;
			}
		}

		layer.textRenderCache.CreateNew(textRenderer, renderData);
		layer.textsDirty = false;
	}

	List<UIRenderPipeline_OpenGL::LayerData>::Iterator UIRenderPipeline_OpenGL::GetNodeLayer(UI2::Node* node)
	{
		UI2::Node* parent = node->GetParent();
		
		if (parent == (UI2::Node*)screen)
			if (layers.Count() > 0)
				return layers.FirstIterator();
			else
				return layers.AddBack();
		else
		{
			uint count = 0;

			VirtualMap<String>::Iterator renderDataIt;
			while (parent != nullptr)
			{
				renderDataIt = parent->otherData.Find("UIRenderPipeline_OpenGL");

				if (!renderDataIt.IsNull())
					break;

				parent = parent->GetParent();

				if (parent == (UI2::Node*)screen)				
					return layers.Count() > 0 ? layers.FirstIterator() : layers.AddBack();				

				++count;
			}

			if (parent == nullptr)
				Debug::Logger::LogFatal("Client", "Something went wrong");

			auto& renderData = *renderDataIt.GetValue<RenderData>(); 			

			auto renderPipelineIt = renderData.renderPipelines.Find(this);

			if (renderPipelineIt.IsNull())
				Debug::Logger::LogFatal("Blaze Engine", "Something went wrong");

			auto renderPipelineData = renderPipelineIt->value;

			auto layerIterator = renderPipelineData.layerIt;

			for (; count != 0; --count)
			{
				++layerIterator;				
				if (layerIterator.IsNull())
					break;
			}

			for (; count != 0; --count)			
				layerIterator = layers.AddBack();							
		
			return layerIterator;
		}
	}

	void UIRenderPipeline_OpenGL::UnsubscribeNode(UI2::Node* node)
	{
		auto renderDataIt = node->otherData.Find("UIRenderPipeline_OpenGL");

		if (renderDataIt.IsNull())
			Debug::Logger::LogFatal("Client", "Something went wrong");

		auto& renderData = *renderDataIt.GetValue<RenderData>();
		UnsubscribeNode(node, renderData);
	}

	void UIRenderPipeline_OpenGL::UnsubscribeNode(UI2::Node* node, RenderData& renderData)
	{
		node->transformUpdatedEventDipatcher.RemoveHandler(renderData);
		renderData.renderPipelines.Erase(this);
		if (renderData.renderPipelines.Count() == 0)
			node->otherData.Erase("UIRenderPipeline_OpenGL");
	}
	UIRenderPipeline_OpenGL::RenderData::RenderPipelineData& UIRenderPipeline_OpenGL::SubscribeNode(UI2::Node* node)
	{		
		auto& renderData = *node->otherData.Insert<RenderData>("UIRenderPipeline_OpenGL").iterator.GetValue<RenderData>();
		return renderData.renderPipelines.Insert(this).iterator->value;		
	}

	void UIRenderPipeline_OpenGL::AddNode(UI2::Node* node)
	{		
		if (UI2::Panel* panel = dynamic_cast<UI2::Panel*>(node))
		{
			auto layerIterator = GetNodeLayer(node);
			auto& layer = *layerIterator;
			auto& renderData = SubscribeNode(node);
			renderData.layerIt = layerIterator;
			renderData.index = layer.panels.Count() - 1;

			layer.panels.Add(panel);

			RecreatePanelRenderData(layer);

		}
		else if (UI2::Image* image = dynamic_cast<UI2::Image*>(node))
		{
			auto layerIterator = GetNodeLayer(node);
			auto& layer = *layerIterator;
			auto& renderData = SubscribeNode(node);
			renderData.layerIt = layerIterator;
			renderData.index = layer.images.Count() - 1;

			layer.images.Add(image);

			RecreateImageRenderData(layer);

		}
		else if (UI2::Text* text = dynamic_cast<UI2::Text*>(node))
		{
			auto layerIterator = GetNodeLayer(node);
			auto& layer = *layerIterator;
			auto& renderData = SubscribeNode(node);
			renderData.layerIt = layerIterator;
			renderData.index = layer.texts.Count() - 1;

			layer.texts.Add(text);

			RecreateTextRenderData(layer);

		}
	}	
	void UIRenderPipeline_OpenGL::RemoveNode(UI2::Node* node)
	{
		auto renderDataIt = node->otherData.Find("UIRenderPipeline_OpenGL");

		if (renderDataIt.IsNull())
			return;

		auto& renderData = *renderDataIt.GetValue<RenderData>();

		auto renderPipelineIt = renderData.renderPipelines.Find(this);

		if (renderPipelineIt.IsNull())
			Debug::Logger::LogFatal("Blaze Engine", "Something went wrong");

		auto& renderPipelineData = renderPipelineIt->value;

		UnsubscribeNode(node, renderData);

		auto& layer = *renderPipelineData.layerIt;

		if (UI2::Panel* panel = dynamic_cast<UI2::Panel*>(node))
		{
			layer.panels.EraseAt(renderPipelineData.index);
			SetNodeRenderDataArrayIndex(layer.panels[renderPipelineData.index], renderPipelineData.index);
			RecreatePanelRenderData(layer);
		}
		else if (UI2::Image* image = dynamic_cast<UI2::Image*>(node))
		{
			layer.images.EraseAt(renderPipelineData.index);
			SetNodeRenderDataArrayIndex(layer.images[renderPipelineData.index], renderPipelineData.index);
			RecreateImageRenderData(layer);
		}
		else if (UI2::Text* text = dynamic_cast<UI2::Text*>(node))
		{
			layer.texts.EraseAt(renderPipelineData.index);
			SetNodeRenderDataArrayIndex(layer.texts[renderPipelineData.index], renderPipelineData.index);
			RecreateTextRenderData(layer);
		}
	}

	void UIRenderPipeline_OpenGL::AddNodeTree(UI2::Node* node)
	{
		AddNode(node);

		for (auto& child : node->GetChildren())
			AddNodeTree(child);
	}

	void UIRenderPipeline_OpenGL::SetNodeRenderDataArrayIndex(UI2::Node* node, uint newIndex)
	{
		auto renderDataIt = node->otherData.Insert<RenderData>("UIRenderPipeline_OpenGL").iterator;

		if (renderDataIt.IsNull())
			return;

		auto& renderData = *renderDataIt.GetValue<RenderData>();

		auto renderPipelineIt = renderData.renderPipelines.Find(this);		
		if (renderPipelineIt.IsNull())
			Debug::Logger::LogFatal("Blaze Engine", "Something went wrong");

		renderPipelineIt->value.index = newIndex;
	}

	void UIRenderPipeline_OpenGL::OnEvent(UI2::NodeCreatedEvent event)
	{
		AddNode(event.node);
	}

	void UIRenderPipeline_OpenGL::OnEvent(UI2::NodeDestroyedEvent event)
	{
		RemoveNode(event.node);
	}
	void UIRenderPipeline_OpenGL::RenderData::OnEvent(UI2::NodeTransformUpdatedEvent event)
	{
		for (auto rp : renderPipelines)
		{
			auto& layer = *rp.value.layerIt;
			auto node = event.node;

			if (UI2::Panel* panel = dynamic_cast<UI2::Panel*>(node))
				layer.panelsDirty = true;
			else if (UI2::Image* image = dynamic_cast<UI2::Image*>(node))
				layer.imagesDirty = true;
			else if (UI2::Text* text = dynamic_cast<UI2::Text*>(node))
				layer.textsDirty = true;
		}
	}
	void UIRenderPipeline_OpenGL::OnEvent(UI2::ScreenDestructionEvent event)
	{
		if (event.screen != screen)
			Debug::Logger::LogFatal("Client", "Something went wrong");

		SetScreen(nullptr);
	}
}