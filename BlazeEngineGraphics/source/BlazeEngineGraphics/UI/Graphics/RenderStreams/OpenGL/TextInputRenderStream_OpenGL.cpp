#include "pch.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderStreams/OpenGL/TextInputRenderStream_OpenGL.h"
#include "BlazeEngineGraphics/UI/Elements/UIInputBox.h"
#include <cmath>

namespace Blaze::Graphics::OpenGL
{	
	TextInputRenderStream_OpenGL::TextInputRenderStream_OpenGL(UI::InputBox* inputBox)
		: inputBox(inputBox)
	{

	}
	void TextInputRenderStream_OpenGL::BeginStream()
	{				
		textPos = inputBox->textNode.GetFinalPosition();
		textSize = inputBox->textNode.GetFinalSize();
		textScale = inputBox->textNode.GetFinalScale();

		textCullingPos = inputBox->textCullingNode.GetFinalPosition();
		textCullingSize = inputBox->textCullingNode.GetFinalSize();

		cursorPos = inputBox->cursorNode.GetFinalPosition();
		cursorSize = inputBox->cursorNode.GetFinalSize();

		float rotation = inputBox->textNode.GetFinalRotation();
		float cos = Math::Cos(rotation);
		float sin = Math::Sin(rotation);
		right = Vec2f(cos, sin);
		up = Vec2f(-sin, cos);

		cursorHandled = !ShouldRenderCursor();
		selectionHandled = !ShouldRenerSelection();

		if (abs(cos) == 1 || abs(sin) == 1)
		{
			textPos.x = std::ceil(textPos.x);
			textPos.y = std::ceil(textPos.y);
			textCullingPos.x = std::ceil(textCullingPos.x);
			textCullingPos.y = std::ceil(textCullingPos.y);
			cursorPos.x = std::ceil(cursorPos.x);
			cursorPos.y = std::ceil(cursorPos.y);
		}
	}
	void* TextInputRenderStream_OpenGL::GetCurrent()
	{						
		if (!cursorHandled)
		{				
			Vec2f pos = cursorPos;
			Vec2f size = cursorSize;						

			rd = {
				.texture = nullptr,
				.uv1 = Vec2f(),
				.uv2 = Vec2f(),
				.pos = pos,
				.right = right * size.x,
				.up = up * size.y,
				.color = 0xf5f5f5ff,
				.blend = 1,
				.alpha = 1
			};

			cursorHandled = true;
			return &rd;
		}
		if (!selectionHandled)
		{			
			float selectionBegin = GetAdvance(inputBox->selectionBeginPos);
			float selectionEnd = GetAdvance(inputBox->selectionEndPos);			

			Vec2f pos1 = textPos + right * selectionBegin + up * -4 * textScale;
			Vec2f pos2 = textPos + right * selectionEnd + up * -4 * textScale;

			float projectedPos1 = std::max(right.DotProduct(pos1 - textCullingPos), 0.0f);
			float projectedPos2 = std::min(right.DotProduct(pos2 - textCullingPos), textCullingSize.x);

			Vec2f pos = textCullingPos + right * projectedPos1;
			Vec2f size = Vec2f(projectedPos2 - projectedPos1, inputBox->textNode.GetPixelFontHeight() * textScale);

			pos.x = std::ceil(pos.x);
			pos.y = std::ceil(pos.y);

			rd = {
				.texture = nullptr,
				.uv1 = Vec2f(),
				.uv2 = Vec2f(),
				.pos = pos,
				.right = right * size.x,
				.up = up * size.y,
				.color = 0xf5f5f588,
				.blend = 1,
				.alpha = 1
			};

			selectionHandled = true;
			return &rd;
		}

		Debug::Breakpoint();		
	}
	void TextInputRenderStream_OpenGL::Advance()
	{		
	}
	bool TextInputRenderStream_OpenGL::IsEmpty()
	{		
		return cursorHandled && selectionHandled || !ShouldRenderCursor() && !ShouldRenerSelection();
	}
	float TextInputRenderStream_OpenGL::GetAdvance(uint characterIndex)
	{
		auto characterData = inputBox->textNode.GetCharacterData();

		if (characterIndex >= characterData.Count())
			return (inputBox->textNode.GetTransform().size.x + 1) * textScale;
		else if (characterIndex == 0)
			return characterData.First().pos.x * textScale;
		else
			return std::ceil((characterData[characterIndex].pos.x +
				characterData[characterIndex - 1].pos.x + characterData[characterIndex - 1].size.x) / 2) * textScale;
	}
	bool TextInputRenderStream_OpenGL::ShouldRenderCursor()
	{
		return std::fmod(TimePoint::GetRunTime() - inputBox->selectedTime, 1.0) < 0.5 && inputBox->selected;
	}
	bool TextInputRenderStream_OpenGL::ShouldRenerSelection()
	{
		return inputBox->selectionBeginPos != inputBox->selectionEndPos && inputBox->selected;
	}
}