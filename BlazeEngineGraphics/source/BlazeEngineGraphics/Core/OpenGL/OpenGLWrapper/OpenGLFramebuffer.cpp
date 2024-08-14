#include "pch.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLFramebuffer.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLConversions.h"

namespace Blaze::Graphics::OpenGLWrapper
{
	Framebuffer::Framebuffer()
		: id(0)
	{
		glGenFramebuffers(1, &id);
		
	}
	Framebuffer::Framebuffer(Framebuffer&& fb) noexcept
		: id(fb.id)
	{
		fb.id = 0;
	}
	Framebuffer::~Framebuffer()
	{
		if (id != 0)		
			glDeleteFramebuffers(1, &id);					
	}	
	void Framebuffer::SetColorAttachment(uint colorAttachmentNumber, Renderbuffer& renderbuffer)
	{		
		glNamedFramebufferRenderbuffer(id, GL_COLOR_ATTACHMENT0 + colorAttachmentNumber, GL_RENDERBUFFER, renderbuffer.GetHandle());		
	}
	void Framebuffer::SetColorAttachment(uint colorAttachmentNumber, Texture2D& texture)
	{			
		glNamedFramebufferTexture(id, GL_COLOR_ATTACHMENT0 + colorAttachmentNumber, texture.GetHandle(), 0);
	}
	void Framebuffer::SetAttachment(FramebufferAttachment attachment, Renderbuffer& renderbuffer)
	{
		Result result;
		GLenum _attachment = OpenGLFramebufferAttachment(attachment, result);
		if (result) return;

		glNamedFramebufferRenderbuffer(id, _attachment, GL_RENDERBUFFER, renderbuffer.GetHandle());	
	}
	void Framebuffer::SetAttachment(FramebufferAttachment attachment, Texture2D& texture)
	{		
		Result result;
		GLenum _attachment = OpenGLFramebufferAttachment(attachment, result);				
		if (result) return;

		glNamedFramebufferTexture(id, _attachment, texture.GetHandle(), 0);		
	}
	FramebufferStatus Framebuffer::GetStatus() const
	{		
		GLenum status = glCheckNamedFramebufferStatus(id, GL_FRAMEBUFFER);
		Result result;
		FramebufferStatus status_ = MapFramebufferStatus(status, result);
		return status_;
	}
	void Framebuffer::SetBufferOutputs(const std::initializer_list<int>& outputs)
	{
		std::vector<GLenum> values;
		values.resize(outputs.size());
		for (int i = 0; i < outputs.size(); ++i)
			if (outputs.begin()[i] == -1)
				values[i] = GL_NONE;
			else
				values[i] = static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + outputs.begin()[i]);		
		
		glNamedFramebufferDrawBuffers(id, static_cast<GLsizei>(outputs.size()), values.data());		
	}
	Framebuffer& Framebuffer::operator=(Framebuffer&& fb) noexcept
	{
		if (id != 0)		
			glDeleteFramebuffers(1, &id);			
		
		id = fb.id;
		fb.id = -1;
		return *this;
	}
}