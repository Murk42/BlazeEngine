#pragma once
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineGraphics/UI/Input/InputNode.h"
#include "BlazeEngineGraphics/UI/Input/MouseEventHandler.h"
#include "BlazeEngineGraphics/UI/Input/KeyboardEventHandler.h"
#include "BlazeEngineGraphics/UI/Input/SelectEventHandler.h"
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
		
		int HitTest(Vec2f screenPosition) override;
		void OnEvent(const DeselectedEvent& event) override;
		void OnEvent(const KeyDownEvent& event) override;		
		void OnEvent(const MouseButtonDownEvent& event) override;
		void OnEvent(const MouseMotionEvent& event) override;
		void OnEvent(const MouseButtonUpEvent& event) override;
		void OnEvent(const MouseEnterEvent& event) override;
		void OnEvent(const MouseExitEvent& event) override;
	private:
		bool mouseDown : 1;		
		uintMem grabbedSelectionBegin;
		uintMem grabbedSelectionEnd;

		void SelectAroundGrabbedSelection(uintMem characterIndex);
	};
}