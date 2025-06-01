#pragma once
#include "BlazeEngineGraphics/RenderScene/RenderUnit.h"

namespace Blaze::UI
{
	class TextCursor;

	class BLAZE_GRAPHICS_API TextCursorRenderUnitBase :
		public Graphics::RenderUnit
	{
	public:
		TextCursor& cursor;

		TextCursorRenderUnitBase(TextCursor& cursor, StringView rendererName);

		virtual bool CleanRenderData() = 0;

		virtual void ShowCursor() = 0;
		virtual void HideCursor() = 0;
	};
}