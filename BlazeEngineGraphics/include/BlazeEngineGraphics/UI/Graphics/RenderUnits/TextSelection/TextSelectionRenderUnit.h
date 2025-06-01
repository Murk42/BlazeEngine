#pragma once
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/TextSelection/TextSelectionRenderUnitBase.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/Text/TextRenderUnitBase.h"
#include "BlazeEngineGraphics/UI/Common/TextSelection.h"
#include "BlazeEngineGraphics/Renderers/TexturedRectRenderer.h"

namespace Blaze::UI
{	
	class BLAZE_GRAPHICS_API TextSelectionRenderUnit :
		public TextSelectionRenderUnitBase
	{
	public:		
		TextRenderUnitBase& textRenderUnit;

		TextSelectionRenderUnit(TextSelection& selection, TextRenderUnitBase& textRenderUnit);
		~TextSelectionRenderUnit();

		void BeginStream() override;
		void* Advance() override;
		
		bool CleanRenderData() override;
	private:
		struct LineSelection 
		{
			Vec2f pos;
			Vec2f right;
			Vec2f up;
		};
		
		Graphics::TexturedRectRenderData renderData;				
		Array<LineSelection> lineSelections;
		uint lineIndex;
		
		bool renderDataDirty;		

		void SelectionChangedEvent(const TextSelection::SelectionChangedEvent& event);
		void RenderDataUpdatedEvent(const TextRenderUnitBase::RenderDataUpdatedEvent& event);
	};
}