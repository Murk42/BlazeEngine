#pragma once
#include "BlazeEngineGraphics/UI/Elements/Text/Cursor/TextCursorRenderUnitBase.h"
#include "BlazeEngineGraphics/UI/Elements/Text/TextRenderUnitBase.h"
#include "BlazeEngineGraphics/Renderers/TexturedRectRenderer.h"

namespace Blaze::UIGraphics
{	
	class BLAZE_GRAPHICS_API TextCursorRenderUnit :
		public TextCursorRenderUnitBase,
		private EventHandler<UIGraphics::TextRenderUnitBase::RenderDataUpdatedEvent>
	{
	public:		
		UIGraphics::TextRenderUnitBase& textRenderUnit;

		TextCursorRenderUnit(UIGraphics::TextRenderUnitBase& textRenderUnit);

		void BeginStream() override;
		void* Advance() override;

		bool CleanData() override;
		bool CleanRenderData() override;

		void SetCursorPos(uintMem lineIndex, uintMem lineCharacterIndex) override;
		//Sets the cursor to be before the character index. If the character index is outside valid range the cursor is set to the end
		void SetCursorPosBeforeCharacter(uintMem characterIndex) override;
		//Sets the cursor to be after the character index. If the character index is outside valid range the cursor is set to the end
		void SetCursorPosAfterCharacter(uintMem characterIndex) override;
		void SetCursorPosToBeginning() override;
		void SetCursorPosToEnd() override;
		//Moves the cursor one character ahead. Returns true if the cursor has successfully advanced, false otherwise.		
		bool AdvanceCursor() override;
		//Moves the cursor one character behind. Returns true if the cursor has successfully retreated, false otherwise. 		
		bool RetreatCursor() override;

		uintMem GetCursorLineIndex() override;
		uintMem GetCursorLineCharacterIndex() override;
		//Returns the index of the character that is after the cursor. If there is no characters after the cursor returns the index of the last character
		uintMem GetIndexOfCharacterAfterCursor() override;
		//Returns the index of the character that is before the cursor. If there is no characters before the cursor returns 0
		uintMem GetIndexOfCharacterBeforeCursor() override;
		bool IsCursorAtEnd() override;
		bool IsCursorAtBeggining() override;

		void HideCursor();
		void ShowCursor();				
		
		inline bool IsCursorShown() const { return shown; }
	private:		
		uintMem cursorLineIndex;
		uintMem cursorLineCharacterIndex;

		Graphics::TexturedRectRenderData renderData;

		TimePoint lastTimeCursorSet;
		bool shown : 1;
		bool culled : 1;
		bool rendered : 1;
		bool renderDataDirty : 1;

		void ValidateCursorPos();
		void OnEvent(UIGraphics::TextRenderUnitBase::RenderDataUpdatedEvent event);
	};
}