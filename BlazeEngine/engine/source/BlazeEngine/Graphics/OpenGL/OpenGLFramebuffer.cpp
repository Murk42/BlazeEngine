#include "BlazeEngine/Graphics/OpenGL/OpenGLFramebuffer.h"
#include "source/BlazeEngine/Internal/Conversions.h"

#include <GL/glew.h>

namespace Blaze::OpenGL
{
	Framebuffer::Framebuffer()
		: id(-1)
	{
		glGenFramebuffers(1, &id);
	}
	Framebuffer::Framebuffer(Framebuffer&& fb) noexcept
		: id(fb.id)
	{
		fb.id = -1;
	}
	Framebuffer::~Framebuffer()
	{
		if (id != -1)
			glDeleteFramebuffers(1, &id);
	}	
	void Framebuffer::SetColorAttachment(uint colorAttachmentNumber, Renderbuffer& renderbuffer)
	{
		glNamedFramebufferRenderbuffer(id, GL_COLOR_ATTACHMENT0 + colorAttachmentNumber, GL_RENDERBUFFER, renderbuffer.GetHandle());
	}
	void Framebuffer::SetColorAttachment(uint colorAttachmentNumber, Texture2D& texture)
	{		
		glBindTexture(GL_TEXTURE_2D, texture.GetHandle());
		glBindFramebuffer(GL_FRAMEBUFFER, id);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachmentNumber, GL_TEXTURE_2D, texture.GetHandle(), 0);
	}
	void Framebuffer::SetAttachment(FramebufferAttachment attachment, Renderbuffer& renderbuffer)
	{
		GLenum _attachment = OpenGLFramebufferAttachment(attachment);		
		glNamedFramebufferRenderbuffer(id, _attachment, renderbuffer.GetHandle(), 0);
	}
	void Framebuffer::SetAttachment(FramebufferAttachment attachment, Texture2D& texture)
	{
		GLenum _attachment = OpenGLFramebufferAttachment(attachment);		
		glNamedFramebufferTexture(id, _attachment, texture.GetHandle(), 0);
	}
	bool Framebuffer::IsComplete() const
	{
		return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
	}
	Framebuffer& Framebuffer::operator=(Framebuffer&& fb) noexcept
	{
		if (id != -1)
			glDeleteFramebuffers(1, &id);
		id = fb.id;
		fb.id = -1;
		return *this;
	}
}