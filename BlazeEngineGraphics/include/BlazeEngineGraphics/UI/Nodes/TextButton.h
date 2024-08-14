#pragma once
#include "BlazeEngineGraphics/UI/Nodes/Text.h"
#include "BlazeEngineGraphics/UI/Nodes/Panel.h"

#include "BlazeEngineGraphics/UI/Input/UIInputNode.h"
#include "BlazeEngineGraphics/UI/Input/UIMouseEventHandler.h"
#include "BlazeEngineGraphics/UI/Input/UISelectEventHandler.h"

#include "BlazeEngineGraphics/RenderScene/RenderObject.h"

namespace Blaze::UI
{
	class TextButton :
		public UIInputNode,
		public UIMouseEventHandler,
		public UISelectEventHandler,
		public Graphics::RenderObject,
		EventHandler<Node::FinalTransformUpdatedEvent>,
		EventHandler<Input::Events::KeyReleased>
	{
	public:
		struct PressedEvent { UIInputManager* inputManager; };
		EventDispatcher<PressedEvent> pressedEventDispatcher;		

		TextButton();
		~TextButton() override;		

		void SetNormalColor(ColorRGBAf color);
		void SetHighlightedColor(ColorRGBAf color);
		void SetPressedColor(ColorRGBAf color);

		inline Text& GetTextNode() { return text; }
		inline Panel& GetPanelNode() { return panel; }

		Graphics::RenderUnit* GetRenderUnit(uint index) override;
	private:
		Panel panel;
		Text text;
		bool hovered = false;
		bool pressed = false;
		bool selected = false;
		ColorRGBAf normalColor = 0x333333ff;
		ColorRGBAf highlightedColor = 0x404040ff;
		ColorRGBAf pressedColor = 0x282828ff;

		void UpdateColor();

		void OnEvent(Node::FinalTransformUpdatedEvent event) override;
		void OnEvent(MouseEnterEvent event) override;
		void OnEvent(MouseExitEvent event) override;
		void OnEvent(MousePressedEvent event) override;
		void OnEvent(SelectedEvent event) override;
		void OnEvent(DeselectedEvent event) override;
		void OnEvent(Input::Events::KeyReleased event) override;				
	};
}