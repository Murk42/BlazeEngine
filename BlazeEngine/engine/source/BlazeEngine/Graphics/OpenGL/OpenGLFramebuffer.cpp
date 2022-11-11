#include "BlazeEngine/Graphics/OpenGL/OpenGLFramebuffer.h"
#include "source/BlazeEngine/Internal/Conversions.h"

#include <GL/glew.h>

#include "BlazeEngine/Graphics/GraphicsCore.h"

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
		Graphics::Core::SelectFramebuffer(this);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachmentNumber, GL_RENDERBUFFER, renderbuffer.GetHandle());
	}
	void Framebuffer::SetColorAttachment(uint colorAttachmentNumber, Texture2D& texture)
	{	
		Graphics::Core::SelectFramebuffer(this);
		Graphics::Core::SelectTexture(&texture);		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachmentNumber, GL_TEXTURE_2D, texture.GetHandle(), 0);
	}
	void Framebuffer::SetAttachment(FramebufferAttachment attachment, Renderbuffer& renderbuffer)
	{
		Graphics::Core::SelectFramebuffer(this);
		GLenum _attachment = OpenGLFramebufferAttachment(attachment);		
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, _attachment, GL_RENDERBUFFER, renderbuffer.GetHandle());
	}
	void Framebuffer::SetAttachment(FramebufferAttachment attachment, Texture2D& texture)
	{
		Graphics::Core::SelectFramebuffer(this);
		GLenum _attachment = OpenGLFramebufferAttachment(attachment);				
		glFramebufferTexture(GL_FRAMEBUFFER, _attachment, texture.GetHandle(), 0);
	}
	bool Framebuffer::IsComplete() const
	{
		return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
	}
	void Framebuffer::SetBufferOutputs(const std::initializer_list<int>& outputs)
	{
		std::vector<GLenum> values;
		values.resize(outputs.size());
		for (int i = 0; i < outputs.size(); ++i)
			if (outputs.begin()[i] == -1)
				values[i] = GL_NONE;
			else
				values[i] = GL_COLOR_ATTACHMENT0 + outputs.begin()[i];
		Graphics::Core::SelectFramebuffer(this);
		glDrawBuffers(outputs.size(), values.data());
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