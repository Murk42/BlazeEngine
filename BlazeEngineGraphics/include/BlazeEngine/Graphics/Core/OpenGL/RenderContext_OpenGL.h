#pragma once
#include "BlazeEngine/Graphics/Core/RenderContext.h"
#include "BlazeEngine/Graphics/Core/OpenGL/FramebufferBase_OpenGL.h"
#include "BlazeEngine/Core/Math/Matrix.h"

namespace Blaze::Graphics::OpenGL
{
	class RenderContext_OpenGL : public RenderContext
	{
	public:
		FramebufferBase_OpenGL& framebuffer;
		Vec2i renderPos;
		Vec2u renderSize;

		RenderContext_OpenGL(FramebufferBase_OpenGL& framebuffer) : framebuffer(framebuffer), renderPos(0), renderSize(framebuffer.GetSize()) {}
		RenderContext_OpenGL(FramebufferBase_OpenGL& framebuffer, Vec2i renderPos, Vec2u renderSize) : framebuffer(framebuffer), renderPos(renderPos), renderSize(renderSize) { }
		RenderContext_OpenGL(const RenderContext_OpenGL& other) = default;
		~RenderContext_OpenGL() override { }

		Mat4f OrthographicsProjectionMatrix(float nearPlane, float farPlane) const
		{
			Vec2f p1 = Vec2f(renderPos);
			Vec2f p2 = p1 + Vec2f(renderSize);
			return Mat4f::OrthographicMatrix(p1.x, p2.x, p1.y, p2.y, -1, 1);
		}
		FramebufferBase& GetFramebuffer() const override { return framebuffer; }
	};
}