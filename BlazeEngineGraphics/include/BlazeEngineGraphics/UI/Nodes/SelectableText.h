#pragma once
#include "BlazeEngineGraphics/UI/Elements/Text/SelectableTextBase.h"
#include "BlazeEngineGraphics/UI/Elements/Text/TextRenderUnit.h"
#include "BlazeEngineGraphics/UI/Elements/Text/Selection/TextSelectionRenderUnit.h"
#include "BlazeEngineGraphics/UI/Elements/Text/Cursor/TextCursorRenderUnit.h"

namespace Blaze::UI
{
	class BLAZE_GRAPHICS_API SelectableText :
		public SelectableTextBase
	{
	public:
		UIGraphics::TextRenderUnit textRenderUnit;
		UIGraphics::TextSelectionRenderUnit textSelectionRenderUnit;		
		UIGraphics::TextCursorRenderUnit textCursorRenderUnit;

		SelectableText();
		
		Graphics::RenderUnit* GetRenderUnit(uint index) override;
	private:
		StringUTF8 GetTextSubString(uintMem begin, uintMem end) override;

	};
}