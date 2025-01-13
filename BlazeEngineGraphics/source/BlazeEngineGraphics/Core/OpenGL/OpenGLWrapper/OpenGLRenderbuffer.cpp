#include "pch.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLRenderbuffer.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLConversions.h"
#include "BlazeEngineGraphics/Core/OpenGL/Debug_OpenGL.h"

namespace Blaze::Graphics::OpenGLWrapper
{
	Renderbuffer::Renderbuffer()		
		: id(0)
	{
		glGenRenderbuffers(1, &id);	
		//glBindRenderbuffer(GL_RENDERBUFFER, id);
	}
	Renderbuffer::Renderbuffer(Renderbuffer&& rb) noexcept
		: id(rb.id)
	{
		rb.id = 0;
	}
	Renderbuffer::~Renderbuffer()
	{
		if (id != 0)		
			glDeleteRenderbuffers(1, &id);
	}
	void Renderbuffer::Create(Vec2u size, Blaze::Graphics::OpenGLWrapper::TextureInternalPixelFormat format)
	{
		Result result;
		GLenum _format = OpenGLInternalPixelFormat(format, result);
		if (result) return;
		
		this->size = size;

		glBindRenderbuffer(GL_RENDERBUFFER, id);
		if (CHECK_OPENGL_ERROR(glRenderbufferStorage(GL_RENDERBUFFER, _format, size.x, size.y)))
		//if (CHECK_OPENGL_ERROR(glNamedRenderbufferStorage(id, _format, size.x, size.y)))
			this->size = Vec2u();
	}
	Renderbuffer& Renderbuffer::operator=(Renderbuffer&& rb) noexcept
	{
		if (id != 0)
			glDeleteRenderbuffers(1, &id);
			
		id = rb.id;
		rb.id = 0;
		return *this;
	}
}