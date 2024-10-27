#pragma once
#include "BlazeEngineCore/BlazeEngineCore.h"
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"

namespace Blaze::Graphics::OpenGLWrapper
{
	class Renderbuffer;
	class Texture2D;
}

namespace Blaze::Graphics::OpenGL
{
	class RenderWindowFramebuffer_OpenGL;

	class BLAZE_GRAPHICS_API Framebuffer_OpenGL
	{
	public:
		Framebuffer_OpenGL();
		Framebuffer_OpenGL(const Framebuffer_OpenGL&) = delete;
		Framebuffer_OpenGL(Framebuffer_OpenGL&&) noexcept;
		/*Parity*/~Framebuffer_OpenGL();

		/*Parity*/inline Vec2u GetSize() const { return size; }
		/*Parity*/inline RenderWindowFramebuffer_OpenGL* GetRenderWindowFramebuffer() { return renderWindowFramebuffer; }

		void SetColorAttachment(uint colorAttachmentNumber, OpenGLWrapper::Renderbuffer& renderbuffer);
		void SetColorAttachment(uint colorAttachmentNumber, OpenGLWrapper::Texture2D& texture);
		void SetAttachment(OpenGLWrapper::FramebufferAttachment attachment, OpenGLWrapper::Renderbuffer& renderbuffer);
		void SetAttachment(OpenGLWrapper::FramebufferAttachment attachment, OpenGLWrapper::Texture2D& texture);
		OpenGLWrapper::FramebufferStatus GetStatus() const;

		void SetBufferOutputs(const std::initializer_list<int>& outputs);

		inline uint GetHandle() const { return id; };		

		Framebuffer_OpenGL& operator=(const Framebuffer_OpenGL&) = delete;
		Framebuffer_OpenGL& operator=(Framebuffer_OpenGL&&) noexcept;
	private:
		uint32 id;
		Vec2u size;
		RenderWindowFramebuffer_OpenGL* renderWindowFramebuffer;

		Framebuffer_OpenGL(RenderWindowFramebuffer_OpenGL* renderWindowFramebuffer);		
		
		friend class RenderWindowFramebuffer_OpenGL;
		friend class RenderWindow_OpenGL;
	};
}