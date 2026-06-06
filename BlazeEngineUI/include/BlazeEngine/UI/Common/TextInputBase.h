#pragma once
#include "BlazeEngine/Core/Event/EventMemberFunctionTie.h"
#include "BlazeEngine/UI/Common/ButtonBase.h"
#include "BlazeEngine/UI/Graphics/RenderableNode.h"
#include "BlazeEngine/UI/Graphics/RenderUnits/StaticTextRenderUnit.h"
#include "BlazeEngine/UI/Graphics/RenderUnits/CursorRenderUnit.h"
#include "BlazeEngine/UI/Text/TextInputControl.h"
#include <BlazeEngine/UI/Text/TextLayoutMetadata.h>

namespace Blaze::UI::Nodes
{
	class TextInputBase : public ButtonBase, public RenderableNode
	{
	public:
		TextInputBase();
		TextInputBase(Node& parent, const NodeTransform& transform);
		TextInputBase(Node& parent, const NodeTransform& transform, TextStyle style);
		~TextInputBase();

		void SetTextStyle(TextStyle style);
		 
		inline TextStyle GetTextStyle() const { return style; }

		bool PreRender(const Graphics::RenderContext& renderContext) override;
		RenderUnitBase* GetRenderUnit(uintMem index) override;
	protected:
		struct TextRenderData
		{
			u8String text;
			TextStyle style;
		};

		virtual u8StringView FindWordAt(u8StringView text, uintMem index);
		virtual u8StringView FindLineAt(u8StringView text, uintMem index);		
		virtual bool IsInsertable(UnicodeChar ch) const;		
		virtual void TextEntered();
		virtual void FilterRenderData(u8String& string, TextStyle& style);
		virtual Array<Text::ShapedString> ShapeText(u8StringView text, const TextStyle& style);

		void InvalidateRenderedText();
		void IsMouseOverChanged(Vec2f pos) override;
		void PressableFlagChanged() override;

		void SetInputedText(u8String text);		
		inline u8StringView GetInputedText() { return inputControl.GetText(); }
	private:
		bool updateTextRenderUnit : 1;
		bool updatetCursorRenderUnit : 1;
		bool inputeStringChanged : 1;

		TextStyle style;

		StaticTextRenderUnit textRenderUnit;
		CursorRenderUnit cursorRenderUnit;

		Text::TextInputControl inputControl;
		Text::TextLayoutMetadata layoutMetadata;
	

		u8String FilterInsertString(u8StringView string);

		bool IsRenderingEmptyText() const;

		void FinalScaleUpdated(const FinalScaleUpdatedEvent& event);
		EVENT_MEMBER_FUNCTION(TextInputBase, FinalScaleUpdated, finalScaleUpdatedEventDispatcher);

		void SelectedStateChanged(const InputNode::SelectedStateChangedEvent& event);
		EVENT_MEMBER_FUNCTION(TextInputBase, SelectedStateChanged, selectedStateChangedEventDispatcher);

		void MouseButtonDownEvent(const UIMouseButtonDownEvent& event);
		EVENT_MEMBER_FUNCTION(TextInputBase, MouseButtonDownEvent, mouseButtonDownEventDispatcher);

		void MouseMotionEvent(const UIMouseMotionEvent& event);
		EVENT_MEMBER_FUNCTION(TextInputBase, MouseMotionEvent, mouseMotionEventDispatcher);

		void KeyDownEvent(const UIKeyDownEvent& event);
		EVENT_MEMBER_FUNCTION(TextInputBase, KeyDownEvent, keyDownEventDispatcher);

		void TextInputEvent(const UITextInputEvent& event);
		EVENT_MEMBER_FUNCTION(TextInputBase, TextInputEvent, textInputEventDispatcher);

		void TextInputControlStateChanged(const Text::TextCursorControl::StateChangedEvent& event);
		EVENT_MEMBER_FUNCTION(TextInputBase, TextInputControlStateChanged, inputControl.GetCursorControl().stateChangedEventDispatcher);
	};
}