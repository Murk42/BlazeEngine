#pragma once
#include "BlazeEngineGraphics/RenderScene/RenderUnit.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/Text/TextRenderUnit.h"

namespace Blaze::UI
{
	class BLAZE_GRAPHICS_API TextSelectionRenderUnitBase :
		public Graphics::RenderUnit
	{
	public:
		TextSelectionRenderUnitBase(StringView rendererName);

		virtual bool CleanData() = 0;
		virtual bool CleanRenderData() = 0;

		virtual void SetSelection(uintMem begin, uintMem end) = 0;
		virtual void SetSelectionBegin(uintMem begin) = 0;
		virtual void SetSelectionEnd(uintMem end) = 0;

		virtual void ClearSelection() = 0;

		virtual uintMem GetSelectionBegin() = 0;
		virtual uintMem GetSelectionEnd() = 0;
		virtual bool IsSelectionEmpty() = 0;
	};
}