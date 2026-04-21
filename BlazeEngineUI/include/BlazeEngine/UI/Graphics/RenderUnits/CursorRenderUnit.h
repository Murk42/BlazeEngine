#pragma once
#include <BlazeEngine/Core/Time/TimePoint.h>

#include "BlazeEngine/Graphics/Renderers/Quad2DRenderer.h"
#include "BlazeEngine/UI/Graphics/RenderUnit.h"
#include <BlazeEngine/UI/Text/TextLayoutMetadata.h>
#include <BlazeEngine/UI/Text/TextInputControl.h>

namespace Blaze::UI::Nodes
{
	class CursorRenderUnit : public RenderUnit<Graphics::Quad2DRenderer>
	{
	public:
		CursorRenderUnit();

		void Clear();
		void SetStyle(Vec2f screenSize, ColorRGBAf color, ColorRGBAf selectionColor);
		void Update(const Text::TextInputControl& inputControl, const Text::TextLayoutMetadata& layoutMetadata);
		void SetHideFlag(bool hide);

		void Render(const Node& node, Graphics::Quad2DRenderer& renderer) override;
	private:
		ProgramTimePoint blinkingStart;
		uintMem prevCursorPos;

		Vec2f nodeRelativePos;
		Vec2f screenSize;
		ColorRGBAf color;
		ColorRGBAf selectionColor;

		struct SelectionBox
		{
			Vec2f pos1;
			Vec2f pos2;
		};
		Array<SelectionBox> selectionBoxes;

		bool hide : 1;

		void UpdateCursor(const Text::TextInputControl& inputControl, const Text::TextLayoutMetadata& layoutMetadata);
		void UpdateSelectionBoxes(const Text::TextInputControl& inputControl, const Text::TextLayoutMetadata& layoutMetadata);
	};
}