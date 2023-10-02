#include "BlazeEngine/Graphics/OpenGL/OpenGLFramebuffer.h"
#include "BlazeEngine/Internal/Conversions.h"

#include <GL/glew.h>

#include "BlazeEngine/Graphics/GraphicsCore.h"

namespace Blaze::OpenGL
{
	Framebuffer::Framebuffer()
		: id(-1)
	{
		glGenFramebuffers(1, &id);
		FlushOpenGLResult();
	}
	Framebuffer::Framebuffer(Framebuffer&& fb) noexcept
		: id(fb.id)
	{
		fb.id = -1;
	}
	Framebuffer::~Framebuffer()
	{
		if (id != -1)
		{
			glDeleteFramebuffers(1, &id);
			FlushOpenGLResult();
		}
	}	
	Result Framebuffer::SetColorAttachment(uint colorAttachmentNumber, Renderbuffer& renderbuffer)
	{
		CHECK_RESULT(Graphics::Renderer::SelectFramebuffer(this));
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachmentNumber, GL_RENDERBUFFER, renderbuffer.GetHandle());
		CHECK_OPENGL_RESULT();

		return Result();
	}
	Result Framebuffer::SetColorAttachment(uint colorAttachmentNumber, Texture2D& texture)
	{	
		CHECK_RESULT(Graphics::Renderer::SelectFramebuffer(this));
		CHECK_RESULT(Graphics::Renderer::SelectTexture(&texture));		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachmentNumber, GL_TEXTURE_2D, texture.GetHandle(), 0);
		CHECK_OPENGL_RESULT();

		return Result();
	}
	Result Framebuffer::SetAttachment(FramebufferAttachment attachment, Renderbuffer& renderbuffer)
	{
		CHECK_RESULT(Graphics::Renderer::SelectFramebuffer(this));

		Result result;
		GLenum _attachment = OpenGLFramebufferAttachment(attachment, result);
		CHECK_RESULT(result);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, _attachment, GL_RENDERBUFFER, renderbuffer.GetHandle());
		CHECK_OPENGL_RESULT();		

		return Result();
	}
	Result Framebuffer::SetAttachment(FramebufferAttachment attachment, Texture2D& texture)
	{
		CHECK_RESULT(Graphics::Renderer::SelectFramebuffer(this));

		Result result;
		GLenum _attachment = OpenGLFramebufferAttachment(attachment, result);				
		CHECK_RESULT(result);

		glFramebufferTexture(GL_FRAMEBUFFER, _attachment, texture.GetHandle(), 0);
		CHECK_OPENGL_RESULT();

		return Result();
	}
	bool Framebuffer::IsComplete() const
	{
		bool ret = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
		FlushOpenGLResult();

		return ret;
	}
	Result Framebuffer::SetBufferOutputs(const std::initializer_list<int>& outputs)
	{
		std::vector<GLenum> values;
		values.resize(outputs.size());
		for (int i = 0; i < outputs.size(); ++i)
			if (outputs.begin()[i] == -1)
				values[i] = GL_NONE;
			else
				values[i] = static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + outputs.begin()[i]);

		CHECK_RESULT(Graphics::Renderer::SelectFramebuffer(this));

		glDrawBuffers(static_cast<GLsizei>(outputs.size()), values.data());
		CHECK_OPENGL_RESULT();

		return Result();
	}
	Framebuffer& Framebuffer::operator=(Framebuffer&& fb) noexcept
	{
		if (id != -1)
		{
			glDeleteFramebuffers(1, &id);
			if (FlushOpenGLResult())
				return *this;
		}
		id = fb.id;
		fb.id = -1;
		return *this;
	}
}