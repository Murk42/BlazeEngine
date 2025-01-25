#pragma once
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineGraphics/UI/Input/UIInputNode.h"
#include "BlazeEngineGraphics/UI/Input/UIMouseEventHandler.h"
#include "BlazeEngineGraphics/UI/Input/UIKeyboardEventHandler.h"
#include "BlazeEngineGraphics/UI/Input/UISelectEventHandler.h"
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

		void OnEvent(SelectedEvent event) override;
		void OnEvent(DeselectedEvent event) override;
		void OnEvent(KeyPressedEvent event) override;
		void OnEvent(TextInputEvent event) override;
		void OnEvent(MousePressedEvent event) override;
		void OnEvent(MouseMotionEvent event) override;
		void OnEvent(MouseReleasedEvent event) override;
		void OnEvent(MouseEnterEvent event) override;
		void OnEvent(MouseExitEvent event) override;
	private:

		bool mouseDown : 1;
		bool multilineInput : 1;
		uintMem grabbedSelectionBegin;
		uintMem grabbedSelectionEnd;		

		void SelectAroundGrabbedSelection(uintMem characterIndex);
		void InsertAtCursor(StringViewUTF8 string);
	};
}