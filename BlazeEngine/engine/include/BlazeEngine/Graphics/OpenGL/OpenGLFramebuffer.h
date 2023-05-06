#pragma once
#include "BlazeEngine/Graphics/OpenGL/Textures/OpenGLTexture2D.h"
#include "BlazeEngine/Graphics/OpenGL/OpenGLRenderbuffer.h"
#include <initializer_list>

namespace Blaze::OpenGL
{
	enum class FramebufferAttachment
	{
		DepthStencil,
		Stencil,
		Depth,
	};

	class BLAZE_API Framebuffer
	{
		unsigned id;
	public:
		Framebuffer();
		Framebuffer(const Framebuffer&) = delete;
		Framebuffer(Framebuffer&&) noexcept;
		~Framebuffer();

		Result SetColorAttachment(uint colorAttachmentNumber, Renderbuffer& renderbuffer);
		Result SetColorAttachment(uint colorAttachmentNumber, Texture2D& texture);
		Result SetAttachment(FramebufferAttachment attachment, Renderbuffer& renderbuffer);
		Result SetAttachment(FramebufferAttachment attachment, Texture2D& texture);
		bool IsComplete() const;

		Result SetBufferOutputs(const std::initializer_list<int>& outputs);

		inline unsigned GetHandle() const { return id; }

		Framebuffer& operator=(const Framebuffer&) = delete;
		Framebuffer& operator=(Framebuffer&&) noexcept;
	};
}