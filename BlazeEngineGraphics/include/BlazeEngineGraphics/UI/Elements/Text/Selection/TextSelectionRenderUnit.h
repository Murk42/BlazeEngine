#pragma once
#include "BlazeEngineGraphics/UI/Elements/Text/Selection/TextSelectionRenderUnitBase.h"
#include "BlazeEngineGraphics/UI/Elements/Text/TextRenderUnit.h"

namespace Blaze::UIGraphics
{	
	class BLAZE_GRAPHICS_API TextSelectionRenderUnit :
		public TextSelectionRenderUnitBase
	{
	public:
		UIGraphics::TextRenderUnitBase& textRenderUnit;

		TextSelectionRenderUnit(UIGraphics::TextRenderUnitBase& textRenderUnit);

		void BeginStream() override;
		void* Advance() override;

		bool CleanData() override;
		bool CleanRenderData() override;

		void SetSelection(uintMem begin, uintMem end) override;
		void SetSelectionBegin(uintMem begin) override;
		void SetSelectionEnd(uintMem end) override;
		
		void ClearSelection() override;
		
		uintMem GetSelectionBegin() override;
		uintMem GetSelectionEnd() override;
		bool IsSelectionEmpty() override;
	private:
		struct LineSelection 
		{
			Vec2f pos;
			Vec2f right;
			Vec2f up;
		};

		uintMem selectionBegin;
		uintMem selectionEnd;
		Graphics::TexturedRectRenderData renderData;				
		Array<LineSelection> lineSelections;
		uint lineIndex;
		
		bool renderDataDirty;
		
		void ValidateSelection();		
	};
}