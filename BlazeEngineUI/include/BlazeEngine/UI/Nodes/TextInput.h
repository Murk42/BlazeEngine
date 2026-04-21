#pragma once
#include "BlazeEngine/Core/Event/EventMemberFunctionTie.h"
#include "BlazeEngine/UI/Common/ButtonBase.h"
#include "BlazeEngine/UI/Graphics/RenderableNode.h"
#include "BlazeEngine/UI/Graphics/RenderUnits/StaticTextRenderUnit.h"
#include "BlazeEngine/UI/Graphics/RenderUnits/CursorRenderUnit.h"
#include "BlazeEngine/UI/Text/TextInputControl.h"
#include <BlazeEngine/UI/Text/TextShaper.h>
#include <BlazeEngine/UI/Text/TextLayoutMetadata.h>
#include <functional>

namespace Blaze::UI::Nodes 
{ 	
	class TextInput : public ButtonBase, public RenderableNode
	{
	public:
		TextInput();
		TextInput(Node& parent, const NodeTransform& transform);
		TextInput(Node& parent, const NodeTransform& transform, const TextStyle& textStyle, u8String emptyText = "", u8String text = "");
		~TextInput();		

		bool SetText(u8String text);
		void SetEmptyText(u8String emptyText);
		void SetTextStyle(const TextStyle& textStyle);
		void SetEmptyTextColor(const ColorRGBAf& color);
		void SetMultiline(bool multiline);
		void SetWrapWidth(float wrapWidth);
		void SetValueEnteredCallback(std::function<void(u8StringView)> callback);
	
		inline u8StringView GetText() { return inputControl.GetText(); }
		inline u8StringView GetEmptyText() { return emptyText; }
		inline const TextStyle& GetTextStyle() const { return textStyle; }		

		bool PreRender(const Graphics::RenderContext& renderContext) override;
		RenderUnitBase* GetRenderUnit(uintMem index) override;
	protected:
		virtual bool FindWordAt(uintMem index, uintMem& begin, uintMem& end);
		virtual bool FindLineAt(uintMem index, uintMem& begin, uintMem& end);

		virtual bool FilterEnteredText(u8StringView text);
	private:
		u8String enteredText;
		u8String emptyText;

		TextStyle textStyle;
		ColorRGBAf emptyTextColor;
		bool multiline : 1;

		bool rebuildGlyphs : 1;
		bool updatetCursorRenderUnit : 1;
		bool updateRenderTopology : 1;

		float textWrapWidth; 

		StaticTextRenderUnit textRenderUnit;
		CursorRenderUnit cursorRenderUnit;
		 
		Text::TextInputControl inputControl;
		Text::TextLayoutMetadata layoutMetadata;

		std::function<void(u8StringView)> callback;

		bool IsRenderingEmptyText() const;
		
		void UpdateTextState();				
		Array<Text::ShapedString> UpdateGlyphs(u8StringView text, const TextStyle& style);

		void FinalScaleUpdated(const FinalScaleUpdatedEvent& event);
		EVENT_MEMBER_FUNCTION(TextInput, FinalScaleUpdated, finalScaleUpdatedEventDispatcher);

		void SelectedStateChanged(const InputNode::SelectedStateChangedEvent& event);
		EVENT_MEMBER_FUNCTION(TextInput, SelectedStateChanged, selectedStateChangedEventDispatcher);

		void MouseButtonDownEvent(const UIMouseButtonDownEvent& event);
		EVENT_MEMBER_FUNCTION(TextInput, MouseButtonDownEvent, mouseButtonDownEventDispatcher);

		void MouseMotionEvent(const UIMouseMotionEvent& event);
		EVENT_MEMBER_FUNCTION(TextInput, MouseMotionEvent, mouseMotionEventDispatcher);

		void KeyDownEvent(const UIKeyDownEvent& event);
		EVENT_MEMBER_FUNCTION(TextInput, KeyDownEvent, keyDownEventDispatcher);

		void TextInputEvent(const UITextInputEvent& event);
		EVENT_MEMBER_FUNCTION(TextInput, TextInputEvent, textInputEventDispatcher);

		void TextInputControlStateChanged(const Text::TextCursorControl::StateChangedEvent& event);
		EVENT_MEMBER_FUNCTION(TextInput, TextInputControlStateChanged, inputControl.GetCursorControl().stateChangedEventDispatcher);
	};
}