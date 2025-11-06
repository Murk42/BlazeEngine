#pragma once
#include "BlazeEngine/Graphics/Renderers/RendererBase.h"
#include "BlazeEngine/Graphics/Renderers/RendererRegistry.h"

#include "BlazeEngine/UI/Core/Screen.h"
#include "BlazeEngine/UI/Graphics/UIRenderUnit.h"

namespace Blaze::UI
{
	class RenderSystem
	{
	public:
		struct RenderGroup
		{
			uintMem count;
			Graphics::RendererBase& renderer;
		};
		struct RenderItem
		{
			UIRenderUnitBase& renderUnit;
			UI::Node& node;
		};

		RenderSystem();
		RenderSystem(Graphics::RendererRegistry rendererRegistry);
		~RenderSystem();

		void SetRendererRegistry(Graphics::RendererRegistry newRegistry);

		void SetScreen(UI::Screen* newScreen);
		void Render(const Graphics::RenderContext& renderContext);

		Graphics::RendererRegistry& GetRendererRegistry() { return rendererRegistry; }

		inline UI::Screen* GetScreen() const { return screen; }
	private:
		Graphics::RendererRegistry rendererRegistry;
		UI::Screen* screen;
		bool recreateRenderQueue;

		Array<RenderItem> renderQueue;
		Array<RenderGroup> renderGroups;

		void RecreateRenderQueue();

		void NodeTreeChanged(const UI::NodeTreeChangedEvent& event);
		void ScreenDestroyed(const UI::ScreenDestructionEvent& event);
		void ChildEnabledStateUpdated(const UI::Node::EnabledStateChangedEvent& event);
	};
}