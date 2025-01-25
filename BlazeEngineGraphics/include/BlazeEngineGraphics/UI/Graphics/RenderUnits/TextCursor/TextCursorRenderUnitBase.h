#pragma once
#include "BlazeEngineGraphics/RenderScene/RenderUnit.h"

namespace Blaze::UI
{
	class BLAZE_GRAPHICS_API TextCursorRenderUnitBase :
		public Graphics::RenderUnit
	{
	public:
		TextCursorRenderUnitBase(StringView rendererName);

		virtual bool CleanData() = 0;
		virtual bool CleanRenderData() = 0;		
		
		//Sets the cursor to be before the character index. If the character index is outside valid range the cursor is set to the end
		virtual void SetCursorPosBeforeCharacter(uintMem characterIndex) = 0;
		//Sets the cursor to be after the character index. If the character index is outside valid range the cursor is set to the end
		virtual void SetCursorPosAfterCharacter(uintMem characterIndex) = 0;
		virtual void SetCursorPosToBeginning() = 0;
		virtual void SetCursorPosToBeginningOfLine(uintMem characterIndex) = 0;
		virtual void SetCursorPosToEnd() = 0;
		virtual void SetCursorPosToEndOfLine(uintMem characterIndex) = 0;
		//Moves the cursor one character ahead. Returns true if the cursor has successfully advanced, false otherwise.		
		virtual bool AdvanceCursor() = 0;
		//Moves the cursor one character behind. Returns true if the cursor has successfully retreated, false otherwise. 		
		virtual bool RetreatCursor() = 0;

		//virtual uintMem GetCursorLineIndex() = 0;
		//virtual uintMem GetCursorLineCharacterIndex() = 0;
		// 
		//Returns the index of the character that is after the cursor. If there is no characters after the cursor returns the index of the last character
		virtual uintMem GetIndexOfCharacterAfterCursor() = 0;
		//Returns the index of the character that is before the cursor. If there is no characters before the cursor returns 0
		//virtual uintMem GetIndexOfCharacterBeforeCursor() = 0;
		virtual bool IsCursorAtEnd() = 0;
		virtual bool IsCursorAtBeggining() = 0;		
	};
}