#include "pch.h"
#include "BlazeEngine/Graphics/Core/OpenGL/Framebuffer_OpenGL.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLRenderbuffer.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/Textures/OpenGLTexture2D.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLConversions.h"
#include "BlazeEngine/Graphics/Core/OpenGL/Debug_OpenGL.h"
#include <GL/glew.h>

namespace Blaze::Graphics::OpenGL
{	
	Framebuffer_OpenGL::Framebuffer_OpenGL()
		: FramebufferBase_OpenGL(0), size(0, 0)
	{
		if (CHECK_OPENGL_ERROR(glGenFramebuffers(1, &id)))
			id = 0;
	}
	Framebuffer_OpenGL::Framebuffer_OpenGL(Framebuffer_OpenGL&& fb) noexcept
		: FramebufferBase_OpenGL(std::move(fb)), size(0, 0)
	{
	}
	Framebuffer_OpenGL::~Framebuffer_OpenGL()
	{		
		glDeleteFramebuffers(1, &id);
	}		
	void Framebuffer_OpenGL::SetColorAttachment(uint colorAttachmentIndex, OpenGL::Renderbuffer& renderbuffer)
	{
		if (id == 0)
		{
			BLAZE_LOG_ERROR("Trying to set a color attachment to an invalid framebuffer object");
			return;
		}
		size = renderbuffer.GetSize();
		glBindFramebuffer(GL_FRAMEBUFFER, id);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachmentIndex, GL_RENDERBUFFER, renderbuffer.GetHandle());
		//glNamedFramebufferRenderbuffer(id, GL_COLOR_ATTACHMENT0 + colorAttachmentIndex, GL_RENDERBUFFER, renderbuffer.GetHandle());
	}
	void Framebuffer_OpenGL::SetColorAttachment(uint colorAttachmentIndex, OpenGL::Texture2D& texture)
	{
		if (id == 0)
		{
			BLAZE_LOG_ERROR("Trying to set a color attachment to an invalid framebuffer object");
			return;
		}

		size = texture.GetSize();
		glNamedFramebufferTexture(id, GL_COLOR_ATTACHMENT0 + colorAttachmentIndex, texture.GetHandle(), 0);
	}
	void Framebuffer_OpenGL::SetAttachment(OpenGL::FramebufferAttachment attachment, OpenGL::Renderbuffer& renderbuffer)
	{
		if (id == 0)
		{
			BLAZE_LOG_ERROR("Trying to set a attachment to an invalid framebuffer object");
			return;
		}

		size = renderbuffer.GetSize();		
		GLenum _attachment; 
		if (!OpenGLFramebufferAttachment(attachment, _attachment))
		{
			BLAZE_LOG_ERROR("Invalid FramebufferAttachment enum value");
			return;
		}

		glNamedFramebufferRenderbuffer(id, _attachment, GL_RENDERBUFFER, renderbuffer.GetHandle());
	}
	void Framebuffer_OpenGL::SetAttachment(OpenGL::FramebufferAttachment attachment, OpenGL::Texture2D& texture)
	{
		if (id == 0)
		{
			BLAZE_LOG_ERROR("Trying to set a attachment to an invalid framebuffer object");
			return;
		}

		size = texture.GetSize();		
		GLenum _attachment;
		if (!OpenGLFramebufferAttachment(attachment, _attachment))
		{
			BLAZE_LOG_ERROR("Invalid FramebufferAttachment enum value");
			return;
		}

		glNamedFramebufferTexture(id, _attachment, texture.GetHandle(), 0);
	}
	OpenGL::FramebufferStatus Framebuffer_OpenGL::GetStatus() const
	{
		if (id == 0)
		{
			BLAZE_LOG_ERROR("Trying to check status of an invalid framebuffer object");
			return OpenGL::FramebufferStatus::Undefined;
		}

		GLenum status = glCheckNamedFramebufferStatus(id, GL_FRAMEBUFFER);
		OpenGL::FramebufferStatus status_;
		if (!BlazeFramebufferStatus(status, status_))
		{
			BLAZE_LOG_ERROR("Invalid framebuffer status value retrieved from OpenGL");
			return OpenGL::FramebufferStatus::Undefined;
		}

		return status_;
	}
	void Framebuffer_OpenGL::SetDrawBuffer(uint targetColorAttachmentNumber)
	{
		if (id == 0)
		{
			BLAZE_LOG_ERROR("Trying to set draw buffer to a invalid framebuffer object");
			return;
		}

		glNamedFramebufferDrawBuffer(id, GL_COLOR_ATTACHMENT0 + targetColorAttachmentNumber);
	}
	void Framebuffer_OpenGL::SetDrawBuffers(const ArrayView<FramebufferDrawBufferInfo>& targets)
	{
		if (id == 0)
		{
			BLAZE_LOG_ERROR("Trying to set draw buffers to invalid framebuffer object");
			return;
		}

		if (targets.Empty())
		{
			glNamedFramebufferDrawBuffers(id, 0, nullptr);
			return;
		}

		uint maxDrawBufferIndex = 0;
		for (auto& target : targets)
			maxDrawBufferIndex = std::max(maxDrawBufferIndex, target.drawBufferIndex);

		Array<GLenum> values;
		values.Resize(static_cast<uintMem>(maxDrawBufferIndex) + 1, GL_NONE);

		for (auto& target : targets)
			values[target.drawBufferIndex] = GL_COLOR_ATTACHMENT0 + target.colorAttachmentIndex;

		glNamedFramebufferDrawBuffers(id, values.Count(), values.Ptr());
	}
	void Framebuffer_OpenGL::DisableDrawBuffers()
	{
		if (id == 0)
		{
			BLAZE_LOG_ERROR("Trying to disable draw buffer to an invalid framebuffer object");
			return;
		}

		glNamedFramebufferDrawBuffer(id, GL_NONE);
	}
	Framebuffer_OpenGL& Framebuffer_OpenGL::operator=(Framebuffer_OpenGL&& fb) noexcept
	{
		if (id != 0)
			glDeleteFramebuffers(1, &id);

		id = fb.id;
		fb.id = 0;
		return *this;
	}
}