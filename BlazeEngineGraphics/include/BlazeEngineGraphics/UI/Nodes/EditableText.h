#pragma once
#include "BlazeEngineGraphics/RenderScene/RenderObject.h"
#include "BlazeEngineGraphics/UI/Common/EditableTextBase.h"
#include "BlazeEngineGraphics/UI/Common/TextSelection.h"
#include "BlazeEngineGraphics/UI/Common/TextCursor.h"
#include "BlazeEngineGraphics/UI/Common/TextContainerReference.h"
#include "BlazeEngineGraphics/UI/Common/StringUTF8TextContainer.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/Text/TextRenderUnit.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/TextSelection/TextSelectionRenderUnit.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/TextCursor/TextCursorRenderUnit.h"

namespace Blaze::UI::Nodes
{			
	class BLAZE_GRAPHICS_API EditableText :
		public EditableTextBase,
		public Graphics::RenderObject
	{
	public:		
		EditableText();
		~EditableText();

		void SetText(StringViewUTF8 text);
		void SetEmptyText(StringViewUTF8 text);				

		inline StringViewUTF8 GetText() const { return textContainer.GetString(); }
		inline StringViewUTF8 GetEmptyText() const { return emptyTextContainer.GetString(); }

		Graphics::RenderUnit* GetRenderUnit(uint index) override;
	private:		
		StringUTF8TextContainer textContainer;
		StringUTF8TextContainer emptyTextContainer;
		TextContainerReference textContainerReference;
		TextSelection selection;
		TextCursor cursor;

		ColorRGBAf emptyColor;
		ColorRGBAf selectedColor;
		ColorRGBAf unselectedColor;		
		
		void MouseHitStatusChangedEvent(const InputNode::MouseHitStatusChangedEvent& event);
		void PressableFlagChangedEvent(const ButtonBase::PressableFlagChangedEvent& event);
		void SelectedStateChangedEvent(const InputNode::SelectedStateChangedEvent& event);		

		void UpdateCursorType();
		void UpdateDisplayedText();
	public:
		TextRenderUnit textRenderUnit;
		TextSelectionRenderUnit textSelectionRenderUnit;
		TextCursorRenderUnit textCursorRenderUnit;
	};
}