#include "pch.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderStreams/OpenGL/ImageRenderStream_OpenGL.h"
#include "BlazeEngineGraphics/UI/Elements/UIImage.h"

namespace Blaze::Graphics::OpenGL
{
	ImageRenderStream_OpenGL::ImageRenderStream_OpenGL(UI::Image* image)
		: image(image), advanced(true)
	{

	}
	void ImageRenderStream_OpenGL::BeginStream()
	{
		advanced = false;
	}
	void* ImageRenderStream_OpenGL::GetCurrent()
	{
		if (advanced)
			return &rd;

		Vec2f size = image->GetFinalSize();
		float rotation = image->GetFinalRotation();
		float cos = Math::Cos(rotation);
		float sin = Math::Sin(rotation);
		Vec2f right = Vec2f(cos, sin) * size.x;
		Vec2f up = Vec2f(-sin, cos) * size.y;

		rd = {
			.texture = image->texture,
			.uv1 = image->uv1,
			.uv2 = image->uv2,
			.pos = image->GetFinalPosition(),
			.right = right,
			.up = up,
			.color = image->color,
			.blend = image->blend,
			.alpha = image->alpha
		};	

		return &rd;
	}
	void ImageRenderStream_OpenGL::Advance()
	{
		advanced = true;
	}
	bool ImageRenderStream_OpenGL::IsEmpty()
	{
		return advanced;
	}
}