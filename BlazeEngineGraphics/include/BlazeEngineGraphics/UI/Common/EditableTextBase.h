#pragma once
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineGraphics/UI/Common/ButtonBase.h"
#include "BlazeEngineGraphics/UI/Common/SelectableTextBase.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/TextCursor/TextCursorRenderUnitBase.h"

namespace Blaze::UI
{
	class BLAZE_GRAPHICS_API EditableTextBase : public SelectableTextBase		
	{
	public:		
		struct TextEnteredEvent
		{
			EditableTextBase& editableText;
			TextContainerBase& text;
		};
		
		TextCursorRenderUnitBase& textCursorRenderUnit;
		EventDispatcher<TextEnteredEvent> textEnteredEventDispatcher;

		EditableTextBase(
			TextRenderUnitBase& textRenderUnit,
			TextSelectionRenderUnitBase& textSelectionRenderUnit,
			TextCursorRenderUnitBase& textCursorRenderUnit
		);
		~EditableTextBase();

		void SetMultilineInput(bool multilineInput);
		inline bool IsMultilineInput() const { return multilineInput; }		
	private:
		bool multilineInput : 1;
		//true whenever the text is changed and a new text entered event should be dispatched
		bool textChanged : 1;


		void SelectedStateChangedEvent(const InputNode::SelectedStateChangedEvent& event);
		void KeyDownEvent(const InputNode::KeyDownEvent& event);		
		void TextInputEvent(const InputNode::TextInputEvent& event);		
		
		void ReplaceText(uintMem index, uintMem count, StringViewUTF8 string);
		void EraseText(uintMem index, uintMem count);		
	};
}