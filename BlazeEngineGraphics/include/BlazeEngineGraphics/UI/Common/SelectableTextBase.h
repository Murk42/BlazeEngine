#pragma once
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineGraphics/UI/Common/ButtonBase.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/Text/TextRenderUnitBase.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/TextSelection/TextSelectionRenderUnitBase.h"

namespace Blaze::UI
{
	class BLAZE_GRAPHICS_API SelectableTextBase : public ButtonBase
	{
	public:		
		TextRenderUnitBase& textRenderUnit;
		TextSelectionRenderUnitBase& textSelectionRenderUnit;				

		SelectableTextBase(			
			TextRenderUnitBase& textRenderUnit,
			TextSelectionRenderUnitBase& textSelectionRenderUnit
		);
		~SelectableTextBase();

		int HitTest(Vec2f screenPosition) override;
	private:
		void SelectedStateChangedEvent(const InputNode::SelectedStateChangedEvent& event);
		void KeyDownEvent(const InputNode::KeyDownEvent& event);
		void MouseMotionEvent(const InputNode::MouseMotionEvent& event);
		void MouseButtonDownEvent(const InputNode::MouseButtonDownEvent& event);		
	};
}