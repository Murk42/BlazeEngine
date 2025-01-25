#pragma once
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineGraphics/UI/Input/UIInputNode.h"
#include "BlazeEngineGraphics/UI/Input/UIMouseEventHandler.h"
#include "BlazeEngineGraphics/UI/Input/UIKeyboardEventHandler.h"
#include "BlazeEngineGraphics/UI/Input/UISelectEventHandler.h"
#include "BlazeEngineGraphics/RenderScene/RenderObject.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/Text/TextRenderUnitBase.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/TextSelection/TextSelectionRenderUnitBase.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/TextCursor/TextCursorRenderUnitBase.h"

namespace Blaze::UI
{
	class BLAZE_GRAPHICS_API SelectableTextBase :
		public InputNode,
		public UIKeyboardEventHandler,
		public UISelectEventHandler,
		public UIMouseEventHandler,
		public Graphics::RenderObject
	{
	public:
		TextRenderUnitBase& textRenderUnit;
		TextSelectionRenderUnitBase& textSelectionRenderUnit;		
		TextCursorRenderUnitBase& textCursorRenderUnit;

		SelectableTextBase(
			TextRenderUnitBase& textRenderUnit,
			TextSelectionRenderUnitBase& textSelectionRenderUnit,
			TextCursorRenderUnitBase& textCursorRenderUnit
		);
	protected:
		virtual StringUTF8 GetTextSubString(uintMem begin, uintMem end) = 0;
		
		bool HitTest(Vec2f screenPosition) override;
		void OnEvent(DeselectedEvent event) override;
		void OnEvent(KeyPressedEvent event) override;		
		void OnEvent(MousePressedEvent event) override;
		void OnEvent(MouseMotionEvent event) override;
		void OnEvent(MouseReleasedEvent event) override;
		void OnEvent(MouseEnterEvent event) override;
		void OnEvent(MouseExitEvent event) override;
	private:
		bool mouseDown : 1;		
		uintMem grabbedSelectionBegin;
		uintMem grabbedSelectionEnd;

		void SelectAroundGrabbedSelection(uintMem characterIndex);
	};
}