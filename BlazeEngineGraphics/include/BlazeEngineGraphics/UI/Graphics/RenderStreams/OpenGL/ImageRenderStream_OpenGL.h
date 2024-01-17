#pragma once
#include "BlazeEngineGraphics/Renderers/OpenGL/TexturedRectRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Core/RenderStream.h"

namespace Blaze::UI
{
	class Image;
}

namespace Blaze::Graphics::OpenGL
{
	class ImageRenderStream_OpenGL :
		public RenderStream
	{
		UI::Image* image;
		bool advanced;
		TexturedRectRenderData_OpenGL rd;
	public:
		ImageRenderStream_OpenGL(UI::Image* image);

		void BeginStream() override;
		void* GetCurrent() override;
		void Advance() override;
		bool IsEmpty() override;
	};
}