#pragma once
#include "BlazeEngineGraphics/UI/Nodes/Text.h"
#include "BlazeEngineGraphics/UI/Nodes/Panel.h"

#include "BlazeEngineGraphics/UI/Input/UIInputNode.h"
#include "BlazeEngineGraphics/UI/Input/UIMouseEventHandler.h"
#include "BlazeEngineGraphics/UI/Input/UISelectEventHandler.h"

#include "BlazeEngineGraphics/RenderScene/RenderObject.h"

namespace Blaze::UI
{	
	class BLAZE_GRAPHICS_API StyleProperty
	{

	public:
		enum class Type
		{
			Float, ColorRGB, ColorRGBA, Uint, Int, Resource, String
		};
	private:
		Type type;
		union {
			float propertyFloat;
			ColorRGBf propertyColorRGB;
			ColorRGBAf propertyColorRGBA;
			uint64 propertyUint;
			int64 propertyInt;
			Resource* resource;
			String string;
		};
	};
	class BLAZE_GRAPHICS_API Style
	{
	public:
		Style();

		void GetProperty(StringView name);
	};

	class BLAZE_GRAPHICS_API TextButton :
		public InputNode,
		public UIMouseEventHandler,
		public UISelectEventHandler,
		public Graphics::RenderObject,
		EventHandler<Node::FinalTransformUpdatedEvent>,
		EventHandler<Input::Events::KeyReleased>
	{
	public:
		struct PressedEvent { InputManager* inputManager; };

		EventDispatcher<PressedEvent> pressedEventDispatcher;		

		TextButton();
		~TextButton() override;		
		
		void SetNormalColor(ColorRGBAf panelColor);
		void SetHighlightedColor(ColorRGBAf panelColor);
		void SetPressedColor(ColorRGBAf panelColor);

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