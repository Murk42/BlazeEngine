#include "pch.h"
#include <BlazeEngine/UI/Graphics/RenderUnits/CursorRenderUnit.h>
#include "BlazeEngine/Core/Time/TimeInterval.h"
#include <cmath>

namespace Blaze::UI::Nodes
{
	CursorRenderUnit::CursorRenderUnit()
		: prevCursorPos(SIZE_MAX), nodeRelativePos({ 0, 0 }), screenSize({ 0, 0 }), hide(false)
	{
	}
	void CursorRenderUnit::Clear()
	{
		this->screenSize = { 0, 0 };
	}
	void CursorRenderUnit::SetStyle(Vec2f screenSize, ColorRGBAf color, ColorRGBAf selectionColor)
	{
		if (screenSize.x < 1.0f)
			screenSize.x = 1.0f;

		this->screenSize = screenSize;
		this->color = color;
		this->selectionColor = selectionColor;
	}
	void CursorRenderUnit::Update(const Text::TextInputControl& inputControl, const Text::TextLayoutMetadata& layoutMetadata)
	{
		UpdateCursor(inputControl, layoutMetadata);
		UpdateSelectionBoxes(inputControl, layoutMetadata);
	}
	void CursorRenderUnit::SetHideFlag(bool hide)
	{
		this->hide = hide;

		if (!hide)
			blinkingStart = { };
	}
	void CursorRenderUnit::Render(const Node& node, Graphics::Quad2DRenderer& renderer)
	{
		if (hide)
			return;

		const NodeFinalTransform& finalTransform = node.GetFinalTransform();
		Vec2f pos = nodeRelativePos + finalTransform.position;

		if (std::fmod((ProgramTimePoint() - blinkingStart).GetSeconds(), 1.0) < 0.5)
		{
			renderer.Render(Graphics::Quad2DRenderData{
				.p1 = pos,
				.p2 = pos + finalTransform.Right() * screenSize.x,
				.p3 = pos + finalTransform.Up() * screenSize.y,
				.color = color
				});
		}

		for (auto& box : selectionBoxes)
			renderer.Render(Graphics::Quad2DRenderData{
						.p1 = finalTransform.position + box.pos1,
						.p2 = finalTransform.position + Vec2f{ box.pos2.x, box.pos1.y},
						.p3 = finalTransform.position + Vec2f{ box.pos1.x, box.pos2.y},
						.color = selectionColor
				});

	}
	void CursorRenderUnit::UpdateCursor(const Text::TextInputControl& inputControl, const Text::TextLayoutMetadata& layoutMetadata)
	{
		uintMem cursorPos = inputControl.GetCursorPos();

		if (cursorPos != prevCursorPos)
		{
			blinkingStart = { };
			prevCursorPos = cursorPos;
		}

		nodeRelativePos = layoutMetadata.GetCursorPosBeforeCharacter(cursorPos);
		nodeRelativePos = Math::Floor(nodeRelativePos - Vec2f(screenSize.x / 2, 0));
	}
	void CursorRenderUnit::UpdateSelectionBoxes(const Text::TextInputControl& inputControl, const Text::TextLayoutMetadata& layoutMetadata)
	{
		uintMem selectionBegin = inputControl.GetSelectionBeginning();
		uintMem selectionEnd = inputControl.GetSelectionEnd();

		selectionBoxes.Clear();		

		if (selectionBegin == selectionEnd)
			return;

		uintMem beginLineIndex = layoutMetadata.GetCharacterData(selectionBegin).lineIndex;
		uintMem endLineIndex = layoutMetadata.GetCharacterData(selectionEnd - 1).lineIndex;

		Vec2f offset = Vec2f(0, -0.125f * screenSize.y);
		Vec2f size = Vec2f(0, 1.25f * screenSize.y);
		
		if (beginLineIndex == endLineIndex)
			selectionBoxes.AddBack(SelectionBox{ 
				.pos1 = offset + layoutMetadata.GetCursorPosBeforeCharacter(selectionBegin),
				.pos2 = offset + layoutMetadata.GetCursorPosAfterCharacter(selectionEnd - 1) + size
				});
		else
		{
			selectionBoxes.AddBack(SelectionBox{
				.pos1 = offset + layoutMetadata.GetCursorPosBeforeCharacter(selectionBegin),
				.pos2 = offset + layoutMetadata.GetCursorPosAfterCharacter(layoutMetadata.GetLineData(beginLineIndex + 1).firstCharacter -1) + size
				});

			for (uintMem i = beginLineIndex + 1; i < endLineIndex; ++i)
				selectionBoxes.AddBack(SelectionBox{
				.pos1 = offset + layoutMetadata.GetCursorPosBeforeCharacter(layoutMetadata.GetLineData(i).firstCharacter),
				.pos2 = offset + layoutMetadata.GetCursorPosAfterCharacter(layoutMetadata.GetLineData(i + 1).firstCharacter - 1) + size
					});

			selectionBoxes.AddBack(SelectionBox{
				.pos1 = offset + layoutMetadata.GetCursorPosBeforeCharacter(layoutMetadata.GetLineData(endLineIndex).firstCharacter),
				.pos2 = offset + layoutMetadata.GetCursorPosAfterCharacter(selectionEnd - 1) + size
				});
		}
	}
}