#pragma once
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/Textures/OpenGLTexture2D.h"
#include "OpenGLRenderbuffer.h"

namespace Blaze::Graphics::OpenGLWrapper
{	
	class BLAZE_GRAPHICS_API Framebuffer
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
		FramebufferStatus GetStatus() const;

		void SetBufferOutputs(const std::initializer_list<int>& outputs);

		inline unsigned GetHandle() const { return id; }

		Framebuffer& operator=(const Framebuffer&) = delete;
		Framebuffer& operator=(Framebuffer&&) noexcept;
	};
}