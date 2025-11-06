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
		: id(-1), renderWindowFramebuffer(nullptr)
	{
		if (CHECK_OPENGL_ERROR(glGenFramebuffers(1, &id)))
			id = -1;
	}
	Framebuffer_OpenGL::Framebuffer_OpenGL(Framebuffer_OpenGL&& fb) noexcept
		: id(fb.id), renderWindowFramebuffer(fb.renderWindowFramebuffer)
	{
		fb.id = -1;
		fb.renderWindowFramebuffer = nullptr;
	}
	Framebuffer_OpenGL::~Framebuffer_OpenGL()
	{		
	}		
	void Framebuffer_OpenGL::SetColorAttachment(uint colorAttachmentNumber, OpenGL::Renderbuffer& renderbuffer)
	{
		size = renderbuffer.GetSize();
		glBindFramebuffer(GL_FRAMEBUFFER, id);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachmentNumber, GL_RENDERBUFFER, renderbuffer.GetHandle());
		//glNamedFramebufferRenderbuffer(id, GL_COLOR_ATTACHMENT0 + colorAttachmentNumber, GL_RENDERBUFFER, renderbuffer.GetHandle());
	}
	void Framebuffer_OpenGL::SetColorAttachment(uint colorAttachmentNumber, OpenGL::Texture2D& texture)
	{
		size = texture.GetSize();
		glNamedFramebufferTexture(id, GL_COLOR_ATTACHMENT0 + colorAttachmentNumber, texture.GetHandle(), 0);
	}
	void Framebuffer_OpenGL::SetAttachment(OpenGL::FramebufferAttachment attachment, OpenGL::Renderbuffer& renderbuffer)
	{
		size = renderbuffer.GetSize();
		Result result;
		GLenum _attachment = OpenGLFramebufferAttachment(attachment, result);
		if (result) return;

		glNamedFramebufferRenderbuffer(id, _attachment, GL_RENDERBUFFER, renderbuffer.GetHandle());
	}
	void Framebuffer_OpenGL::SetAttachment(OpenGL::FramebufferAttachment attachment, OpenGL::Texture2D& texture)
	{
		size = texture.GetSize();
		Result result;
		GLenum _attachment = OpenGLFramebufferAttachment(attachment, result);
		if (result) return;

		glNamedFramebufferTexture(id, _attachment, texture.GetHandle(), 0);
	}
	OpenGL::FramebufferStatus Framebuffer_OpenGL::GetStatus() const
	{
		GLenum status = glCheckNamedFramebufferStatus(id, GL_FRAMEBUFFER);
		Result result;
		OpenGL::FramebufferStatus status_ = OpenGL::MapFramebufferStatus(status, result);
		return status_;
	}
	void Framebuffer_OpenGL::SetBufferOutputs(const std::initializer_list<int>& outputs)
	{
		Array<GLenum> values;
		values.Resize(outputs.size());
		for (int i = 0; i < outputs.size(); ++i)
			if (outputs.begin()[i] == -1)
				values[i] = GL_NONE;
			else
				values[i] = static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + outputs.begin()[i]);

		glNamedFramebufferDrawBuffers(id, values.Count(), values.Ptr());
	}
	Framebuffer_OpenGL& Framebuffer_OpenGL::operator=(Framebuffer_OpenGL&& fb) noexcept
	{
		if (id != 0)
			glDeleteFramebuffers(1, &id);

		id = fb.id;
		fb.id = -1;
		return *this;
	}
	Framebuffer_OpenGL::Framebuffer_OpenGL(RenderWindowFramebuffer_OpenGL* renderWindowFramebuffer)
		: id(0), renderWindowFramebuffer(renderWindowFramebuffer)
	{
	}
}