#pragma once
#include "BlazeEngineGraphics/UI/Elements/UIText.h"
#include "BlazeEngineGraphics/UI/Elements/UIPanel.h"

#include "BlazeEngineGraphics/UI/Input/UIInputNode.h"
#include "BlazeEngineGraphics/UI/Input/UIMouseEventHandler.h"
#include "BlazeEngineGraphics/UI/Input/UIKeyboardEventHandler.h"
#include "BlazeEngineGraphics/UI/Input/UISelectEventHandler.h"

#include "BlazeEngineGraphics/UI/Graphics/RenderStreams/OpenGL/TextInputRenderStream_OpenGL.h"

namespace Blaze::UI
{
	struct InputBoxValueEnteredEvent
	{
		InputBox* inputBox;
		StringUTF8& text;
	};	

	class InputBox :
		public UIInputNode,
		public UIKeyboardEventHandler,
		public UISelectEventHandler,
		public UIMouseEventHandler,
		EventHandler<NodeTransformUpdatedEvent>
	{
	public:				
		enum class TextAlign
		{
			Left, Center, Right
		};
		
		EventDispatcher<InputBoxValueEnteredEvent> valueEnteredEventDispatcher;

		InputBox(Node* parent, const NodeTransform& transform);
		
		void SetText(StringViewUTF8 text);
		void SetEmptyText(StringViewUTF8 text);

		inline Text& GetTextNode() { return textNode; }
		inline Panel& GetPanelNode() { return panelNode; }

		StringViewUTF8 GetText();

		void SetNormalColor(ColorRGBAf color);
		void SetHighlightedColor(ColorRGBAf color);
		void SetPressedColor(ColorRGBAf color);
		void SetTextEmptyColor(ColorRGBAf color);
		void SetTextEditingColor(ColorRGBAf color);
		void SetTextNormalColor(ColorRGBAf color);
		void SetTextAlign(TextAlign align);
	private:
		Graphics::OpenGL::TextInputRenderStream_OpenGL renderStream;

		Panel panelNode;
		Node textCullingNode;
		Text textNode;
		Node cursorNode;

		uint cursorPos;
		uint selectionBeginPos;
		uint selectionEndPos;

		StringUTF8 text;
		StringUTF8 emptyText;		
		double selectedTime;

		bool mouseDown = true;
		uint mouseDownSelectionBeginPos;
		uint mouseDownSelectionEndPos;

		bool hovered = false;
		bool pressed = false;
		bool selected = false;

		ColorRGBAf panelNormalColor = 0x333333ff;
		ColorRGBAf panelHighlightedColor = 0x373737ff;
		ColorRGBAf panelPressedColor = 0x303030ff;
		ColorRGBAf textEmptyColor = 0xffffff22;
		ColorRGBAf textEditingColor = 0xf5f5f5ff;
		ColorRGBAf textNormalColor = 0xd0d0d0ff;

		TextAlign textAlign = TextAlign::Center;
		float textViewOffset = 0.0f;

		void UpdatePanelColor();
		void UpdateTextColor();			
		//Updates the cursor position and positions the text accordingly
		void UpdateCursorAndText();

		uint GetScreenPositionClosestCharacterBarrier(Vec2f pos);
		uint GetScreenPositionClosestCharacter(Vec2f pos);
		float GetCharacterAdvance(uint characterIndex);
		void FindWord(uint index, uint& begin, uint& end);


		void OnEvent(NodeTransformUpdatedEvent event) override;		
		void OnEvent(SelectedEvent event) override;
		void OnEvent(DeselectedEvent event) override;
		void OnEvent(KeyPressedEvent event) override;
		void OnEvent(TextInputEvent event) override;
		void OnEvent(MousePressedEvent event) override;
		void OnEvent(MouseMotionEvent event) override;
		void OnEvent(MouseReleasedEvent event) override;
		void OnEvent(MouseEnterEvent event) override;
		void OnEvent(MouseExitEvent event) override;

		friend class Graphics::OpenGL::TextInputRenderStream_OpenGL;
	};	

	//template<std::integral T>
	//class InputBoxValueIntegerFilter : public EventHandler<InputBoxValueEnteredEvent>
	//{
	//public:
	//	T value;
	//
	//	void OnEvent(InputBoxValueEnteredEvent event) override
	//	{
	//		StringParsing::Convert(event.inputBox,)
	//	}
	//};
}