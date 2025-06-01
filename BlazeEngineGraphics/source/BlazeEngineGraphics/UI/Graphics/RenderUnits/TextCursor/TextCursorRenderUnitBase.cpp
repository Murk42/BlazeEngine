#include "pch.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/TextCursor/TextCursorRenderUnitBase.h"

namespace Blaze::UI
{
	TextCursorRenderUnitBase::TextCursorRenderUnitBase(TextCursor& cursor, StringView rendererName)
		: RenderUnit(rendererName), cursor(cursor)
	{
	}
}