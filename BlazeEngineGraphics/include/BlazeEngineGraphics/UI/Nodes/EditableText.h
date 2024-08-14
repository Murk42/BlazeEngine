#pragma once
#include "BlazeEngineGraphics/UI/Elements/Text/EditableTextBase.h"
#include "BlazeEngineGraphics/UI/Elements/Text/TextRenderUnit.h"
#include "BlazeEngineGraphics/UI/Elements/Text/Selection/TextSelectionRenderUnit.h"
#include "BlazeEngineGraphics/UI/Elements/Text/Cursor/TextCursorRenderUnit.h"

namespace Blaze::UI
{			
	class BLAZE_GRAPHICS_API EditableText :
		public EditableTextBase
	{
	public:
		UIGraphics::TextRenderUnit textRenderUnit;
		UIGraphics::TextSelectionRenderUnit textSelectionRenderUnit;
		UIGraphics::TextCursorRenderUnit textCursorRenderUnit;

		EditableText();

		void SetText(StringViewUTF8 text);
		void SetEmptyText(StringViewUTF8 text);				

		inline StringViewUTF8 GetText() const { return text; }
		inline StringViewUTF8 GetEmptyText() const { return emptyText; }

		Graphics::RenderUnit* GetRenderUnit(uint index) override;
	private:
		bool selected : 1;
		bool textEdited : 1;

		StringUTF8 text;
		StringUTF8 emptyText;

		ColorRGBAf emptyColor;
		ColorRGBAf selectedColor;
		ColorRGBAf unselectedColor;

		void EraseTextSubString(uintMem begin, uintMem count) override;
		void InsertStringIntoText(uintMem index, StringViewUTF8 string) override;
		StringUTF8 GetTextSubString(uintMem begin, uintMem end) override;

		void UpdateTextRenderUnit();
		
		void OnEvent(SelectedEvent event) override;
		void OnEvent(DeselectedEvent event) override;		
	};
}