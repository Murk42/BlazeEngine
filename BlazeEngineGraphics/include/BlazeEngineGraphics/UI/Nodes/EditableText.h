#pragma once
#include "BlazeEngineGraphics/UI/Common/EditableTextBase.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/Text/TextRenderUnit.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/TextSelection/TextSelectionRenderUnit.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/TextCursor/TextCursorRenderUnit.h"

namespace Blaze::UI::Nodes
{			
	class BLAZE_GRAPHICS_API EditableText :
		public EditableTextBase
	{
	public:
		TextRenderUnit textRenderUnit;
		TextSelectionRenderUnit textSelectionRenderUnit;
		TextCursorRenderUnit textCursorRenderUnit;

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
		
		void OnEvent(const SelectedEvent& event) override;
		void OnEvent(const DeselectedEvent& event) override;		
	};
}