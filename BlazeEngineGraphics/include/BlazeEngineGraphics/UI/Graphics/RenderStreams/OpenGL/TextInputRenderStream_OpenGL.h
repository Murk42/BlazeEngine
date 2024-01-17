#pragma once
#include "BlazeEngineGraphics/Renderers/OpenGL/TexturedRectRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Core/RenderStream.h"

namespace Blaze::UI
{
	class InputBox;
}

namespace Blaze::Graphics::OpenGL
{
	class TextInputRenderStream_OpenGL :
		public RenderStream
	{
	public:
		TextInputRenderStream_OpenGL(UI::InputBox* inputBox);

		void BeginStream() override;
		void* GetCurrent() override;
		void Advance() override;
		bool IsEmpty() override;
	private:
		UI::InputBox* inputBox;
		bool cursorHandled;
		bool selectionHandled;
		TexturedRectRenderData_OpenGL rd;

		Vec2f textPos;
		Vec2f textSize;
		float textScale;

		Vec2f textCullingPos;
		Vec2f textCullingSize;

		Vec2f cursorPos;
		Vec2f cursorSize;

		Vec2f right;
		Vec2f up;

		float GetAdvance(uint characterIndex);
		bool ShouldRenderCursor();
		bool ShouldRenerSelection();
	};
}