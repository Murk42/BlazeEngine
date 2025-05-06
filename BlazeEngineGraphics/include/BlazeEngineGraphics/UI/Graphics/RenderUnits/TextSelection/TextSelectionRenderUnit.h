#pragma once
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/TextSelection/TextSelectionRenderUnitBase.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/Text/TextRenderUnit.h"

namespace Blaze::UI
{	
	class BLAZE_GRAPHICS_API TextSelectionRenderUnit :
		public TextSelectionRenderUnitBase,
		private EventHandler<TextRenderUnitBase::RenderDataUpdatedEvent>
	{
	public:
		TextRenderUnitBase& textRenderUnit;

		TextSelectionRenderUnit(TextRenderUnitBase& textRenderUnit);
		~TextSelectionRenderUnit();

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

		void OnEvent(const TextRenderUnitBase::RenderDataUpdatedEvent& event);

		template<typename T>
		friend class EventHandlerData;
	};
}