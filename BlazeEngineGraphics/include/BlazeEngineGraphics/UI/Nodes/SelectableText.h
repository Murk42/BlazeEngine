#pragma once
#include "BlazeEngineGraphics/RenderScene/RenderObject.h"
#include "BlazeEngineGraphics/UI/Common/SelectableTextBase.h"
#include "BlazeEngineGraphics/UI/Common/TextSelection.h"
#include "BlazeEngineGraphics/UI/Common/StringUTF8TextContainer.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/Text/TextRenderUnit.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/TextSelection/TextSelectionRenderUnit.h"

namespace Blaze::UI::Nodes
{
	class BLAZE_GRAPHICS_API SelectableText :
		public SelectableTextBase,
		public Graphics::RenderObject
	{
	public:

		SelectableText();
		~SelectableText();

		void SetText(StringViewUTF8 text);

		inline StringViewUTF8 GetText() const { return textContainer.GetString(); }
		
		Graphics::RenderUnit* GetRenderUnit(uint index) override;	
	private:
		StringUTF8TextContainer textContainer;
		TextSelection selection;		
	
		void MouseHitStatusChangedEvent(const InputNode::MouseHitStatusChangedEvent& event);
	public:
		TextRenderUnit textRenderUnit;
		TextSelectionRenderUnit textSelectionRenderUnit;
	};
}