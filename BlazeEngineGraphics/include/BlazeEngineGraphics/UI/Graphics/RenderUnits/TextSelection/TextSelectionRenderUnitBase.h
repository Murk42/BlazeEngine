#pragma once
#include "BlazeEngineGraphics/RenderScene/RenderUnit.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/Text/TextRenderUnit.h"

namespace Blaze::UI
{	
	class TextSelection;

	class BLAZE_GRAPHICS_API TextSelectionRenderUnitBase :
		public Graphics::RenderUnit
	{
	public:
		TextSelection& selection;

		TextSelectionRenderUnitBase(TextSelection& selection, StringView rendererName);

		virtual bool CleanRenderData() = 0;
	};
}