#pragma once
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/TextCursor/TextCursorRenderUnitBase.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/Text/TextRenderUnitBase.h"
#include "BlazeEngineGraphics/Renderers/TexturedRectRenderer.h"

namespace Blaze::UI
{	
	class BLAZE_GRAPHICS_API TextCursorRenderUnit :
		public TextCursorRenderUnitBase,
		private EventHandler<TextRenderUnitBase::RenderDataUpdatedEvent>
	{
	public:		
		TextRenderUnitBase& textRenderUnit;

		TextCursorRenderUnit(TextRenderUnitBase& textRenderUnit);
		~TextCursorRenderUnit();

		void BeginStream() override;
		void* Advance() override;

		bool CleanData() override;
		bool CleanRenderData() override;
		
		//Sets the cursor to be before the character index. If the character index is outside valid range the cursor is set to the end
		void SetCursorPosBeforeCharacter(uintMem characterIndex) override;
		//Sets the cursor to be after the character index. If the character index is outside valid range the cursor is set to the end
		void SetCursorPosAfterCharacter(uintMem characterIndex) override;
		void SetCursorPosToBeginning() override;
		void SetCursorPosToBeginningOfLine(uintMem characterIndex) override;
		void SetCursorPosToEnd() override;
		void SetCursorPosToEndOfLine(uintMem characterIndex) override;
		//Moves the cursor one character ahead. Returns true if the cursor has successfully advanced, false otherwise.		
		bool AdvanceCursor() override;
		//Moves the cursor one character behind. Returns true if the cursor has successfully retreated, false otherwise. 		
		bool RetreatCursor() override;
		
		//Returns the index of the character that is after the cursor. If there is no characters after the cursor returns the index of the last character
		uintMem GetIndexOfCharacterAfterCursor() override;
		//Returns the index of the character that is before the cursor. If there is no characters before the cursor returns 0
		//uintMem GetIndexOfCharacterBeforeCursor() override;
		bool IsCursorAtEnd() override;
		bool IsCursorAtBeggining() override;

		void HideCursor();
		void ShowCursor();				
		
		inline bool IsCursorShown() const { return shown; }
	private:		
		uintMem cursorCharacterIndex;		

		Graphics::TexturedRectRenderData renderData;

		TimePoint lastTimeCursorSet;
		bool shown : 1;
		bool culled : 1;
		bool rendered : 1;
		bool renderDataDirty : 1;

		void ValidateCursorPos();
		void OnEvent(const TextRenderUnitBase::RenderDataUpdatedEvent& event);
	};
}