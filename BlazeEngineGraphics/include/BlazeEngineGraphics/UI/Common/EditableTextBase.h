#pragma once
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineGraphics/UI/Input/InputNode.h"
#include "BlazeEngineGraphics/UI/Input/MouseEventHandler.h"
#include "BlazeEngineGraphics/UI/Input/KeyboardEventHandler.h"
#include "BlazeEngineGraphics/UI/Input/SelectEventHandler.h"
#include "BlazeEngineGraphics/RenderScene/RenderObject.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/TextSelection/TextSelectionRenderUnitBase.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/TextCursor/TextCursorRenderUnitBase.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/Text/TextRenderUnit.h"

namespace Blaze::UI
{
	class BLAZE_GRAPHICS_API EditableTextBase :
		public InputNode,
		public UIKeyboardEventHandler,
		public UISelectEventHandler,
		public UIMouseEventHandler,
		public Graphics::RenderObject
	{
	public:		
		struct TextChangedEvent
		{
			EditableTextBase* editableText;
			StringUTF8 string;
		};
		struct TextEnteredEvent
		{
			EditableTextBase* editableText;
			StringUTF8 string;
		};

		TextRenderUnitBase& textRenderUnit;
		TextSelectionRenderUnitBase& textSelectionRenderUnit;
		TextCursorRenderUnitBase& textCursorRenderUnit;
		EventDispatcher<TextChangedEvent> textChangedEventDispatcher;
		EventDispatcher<TextEnteredEvent> textEnteredEventDispatcher;

		EditableTextBase(
			TextRenderUnitBase& textRenderUnit,
			TextSelectionRenderUnitBase& textSelectionRenderUnit,
			TextCursorRenderUnitBase& textCursorRenderUnit
		);

		void SetMultilineInput(bool multilineInput);

		inline bool IsMultilineInput() const { return multilineInput; }
	protected:
		virtual void EraseTextSubString(uintMem begin, uintMem count) = 0;
		virtual void InsertStringIntoText(uintMem index, StringViewUTF8 string) = 0;
		virtual StringUTF8 GetTextSubString(uintMem begin, uintMem end) = 0;

		void OnEvent(const SelectedEvent& event) override;
		void OnEvent(const DeselectedEvent& event) override;
		void OnEvent(const KeyDownEvent& event) override;
		void OnEvent(const TextInputEvent& event) override;
		void OnEvent(const MouseButtonDownEvent& event) override;
		void OnEvent(const MouseMotionEvent& event) override;
		void OnEvent(const MouseButtonUpEvent& event) override;
		void OnEvent(const MouseEnterEvent& event) override;
		void OnEvent(const MouseExitEvent& event) override;
	private:

		bool mouseDown : 1;
		bool multilineInput : 1;
		uintMem grabbedSelectionBegin;
		uintMem grabbedSelectionEnd;		

		void SelectAroundGrabbedSelection(uintMem characterIndex);
		void InsertAtCursor(StringViewUTF8 string);
	};
}