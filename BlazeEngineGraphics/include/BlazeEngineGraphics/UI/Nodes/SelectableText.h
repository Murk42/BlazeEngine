#pragma once
#include "BlazeEngineGraphics/UI/Common/SelectableTextBase.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/Text/TextRenderUnit.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/TextSelection/TextSelectionRenderUnit.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/TextCursor/TextCursorRenderUnit.h"

namespace Blaze::UI::Nodes
{
	class BLAZE_GRAPHICS_API SelectableText :
		public SelectableTextBase
	{
	public:
		TextRenderUnit textRenderUnit;
		TextSelectionRenderUnit textSelectionRenderUnit;		
		TextCursorRenderUnit textCursorRenderUnit;

		SelectableText();
		
		Graphics::RenderUnit* GetRenderUnit(uint index) override;
	private:
		StringUTF8 GetTextSubString(uintMem begin, uintMem end) override;

	};
}