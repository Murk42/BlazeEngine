#pragma once
#include "BlazeEngineCore/Utilities/TimePoint.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/TextCursor/TextCursorRenderUnitBase.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/Text/TextRenderUnitBase.h"
#include "BlazeEngineGraphics/UI/Common/TextCursor.h"
#include "BlazeEngineGraphics/Renderers/TexturedRectRenderer.h"

namespace Blaze::UI
{		
	class BLAZE_GRAPHICS_API TextCursorRenderUnit :
		public TextCursorRenderUnitBase	
	{
	public:				
		TextRenderUnitBase& textRenderUnit;

		TextCursorRenderUnit(TextCursor& cursor, TextRenderUnitBase& textRenderUnit);
		~TextCursorRenderUnit();

		void BeginStream() override;
		void* Advance() override;
		
		bool CleanRenderData() override;		

		void HideCursor() override;
		void ShowCursor() override;				
		
		inline bool IsCursorShown() const { return shown; }
	private:
		Graphics::TexturedRectRenderData renderData;

		TimePoint lastTimeCursorSet;
		bool shown : 1;
		bool culled : 1;
		bool rendered : 1;
		bool renderDataDirty : 1;
		
		void CursorMovedEvent(const TextCursor::CursorMovedEvent& event);
		void TextRenderDataUpdatedEvent(const TextRenderUnitBase::RenderDataUpdatedEvent& event);
	};
}