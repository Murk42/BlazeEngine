#include "pch.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/TextSelection/TextSelectionRenderUnitBase.h"

namespace Blaze::UI
{
	TextSelectionRenderUnitBase::TextSelectionRenderUnitBase(TextSelection& selection, StringView rendererName) :
		RenderUnit(rendererName), selection(selection)
	{
	}
}