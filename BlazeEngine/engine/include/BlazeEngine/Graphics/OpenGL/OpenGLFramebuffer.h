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

		void SetColorAttachment(uint colorAttachmentNumber, Renderbuffer& renderbuffer);
		void SetColorAttachment(uint colorAttachmentNumber, Texture2D& texture);
		void SetAttachment(FramebufferAttachment attachment, Renderbuffer& renderbuffer);
		void SetAttachment(FramebufferAttachment attachment, Texture2D& texture);
		bool IsComplete() const;

		void SetBufferOutputs(const std::initializer_list<int>& outputs);

		inline unsigned GetHandle() const { return id; }

		Framebuffer& operator=(const Framebuffer&) = delete;
		Framebuffer& operator=(Framebuffer&&) noexcept;
	};
}