#pragma once
#include "BlazeEngine/Core/Math/Vector.h"
#include "BlazeEngine/Core/Container/ArrayView.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"
#include "BlazeEngine/Graphics/Core/OpenGL/FramebufferBase_OpenGL.h"

namespace Blaze::Graphics::OpenGL
{
	class Renderbuffer;
	class Texture2D;
}

namespace Blaze::Graphics::OpenGL
{
	struct FramebufferDrawBufferInfo
	{
		uint drawBufferIndex;
		uint colorAttachmentIndex;
	};

	class RenderWindowFramebuffer_OpenGL;

	class BLAZE_API Framebuffer_OpenGL : public FramebufferBase_OpenGL
	{
	public:
		Framebuffer_OpenGL();
		Framebuffer_OpenGL(const Framebuffer_OpenGL&) = delete;
		Framebuffer_OpenGL(Framebuffer_OpenGL&&) noexcept;
		~Framebuffer_OpenGL() override;

		inline Vec2u GetSize() const override { return size; }

		void SetColorAttachment(uint colorAttachmentIndex, OpenGL::Renderbuffer& renderbuffer);
		void SetColorAttachment(uint colorAttachmentIndex, OpenGL::Texture2D& texture);
		void SetAttachment(OpenGL::FramebufferAttachment attachment, OpenGL::Renderbuffer& renderbuffer);
		void SetAttachment(OpenGL::FramebufferAttachment attachment, OpenGL::Texture2D& texture);
		OpenGL::FramebufferStatus GetStatus() const;

		void SetDrawBuffer(uint colorAttachmentIndex);
		void SetDrawBuffers(const ArrayView<FramebufferDrawBufferInfo>& drawBuffers);
		void DisableDrawBuffers();

		Framebuffer_OpenGL& operator=(const Framebuffer_OpenGL&) = delete;
		Framebuffer_OpenGL& operator=(Framebuffer_OpenGL&&) noexcept;
	protected:
		Vec2u size;
	};
}