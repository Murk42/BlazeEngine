#pragma once
#include "BlazeEngine/Graphics/Core/RendererBase.h"
#include "BlazeEngine/Graphics/Core/RendererRegistry.h"
#include "BlazeEngine/UI/Graphics/RenderUnit.h"
#include "BlazeEngine/UI/Core/Screen.h"

namespace Blaze::UI
{
	class GraphicsSubSystem
	{
	public:
		struct RenderGroup
		{
			uintMem count;
			Graphics::RendererBase& renderer;
		};
		struct RenderItem
		{
			RenderUnitBase& renderUnit;
			UI::Node& node;
		};

		GraphicsSubSystem();
		GraphicsSubSystem(Graphics::RendererRegistry rendererRegistry);
		~GraphicsSubSystem();

		void Initialize(Graphics::GraphicsContext& graphicsContext, Graphics::RendererRegistry newRegistry);

		void SetScreen(UI::Screen* newScreen);
		void Render(const Graphics::RenderContext& renderContext);

		const Graphics::RendererRegistry& GetRendererRegistry() const { return rendererRegistry; }

		inline UI::Screen* GetScreen() const { return screen; }
	private:
		Graphics::RendererRegistry rendererRegistry;
		UI::Screen* screen;
		bool recreateRenderQueue;

		Array<RenderItem> renderQueue;
		Array<RenderGroup> renderGroups;

		VirtualList<Graphics::RendererBase> renderers;

		void RecreateRenderQueue();

		void NodeTreeChanged(const Screen::ScreenTreeChangedEvent& event);
		void ScreenDestroyed(const Screen::DestructionEvent& event);
		void ChildEnabledStateUpdated(const UI::Node::NodeEnabledStateChangedEvent& event);
	};
}