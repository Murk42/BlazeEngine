#include "pch.h"
#include "GL/glew.h"
#include "OpenGLContext.h"
#include "OpenGLConversions.h"

namespace OpenGLWrapper
{
	Renderbuffer::Renderbuffer()		
		: id(-1)
	{
		glGenRenderbuffers(1, &id);		

		if (FlushOpenGLResult())
			return;
		
		SelectRenderbuffer(this);

		glBindRenderbuffer(GL_RENDERBUFFER, id);		
		
	}
	Renderbuffer::Renderbuffer(Renderbuffer&& rb) noexcept
		: id(rb.id)
	{
		rb.id = -1;
	}
	Renderbuffer::~Renderbuffer()
	{
		if (id != -1)
		{
			glDeleteRenderbuffers(1, &id);
			
		}
	}
	Result Renderbuffer::Create(Vec2i size, OpenGLWrapper::TextureInternalPixelFormat format)
	{
		this->size = size;

		Result result;
		GLenum _format = OpenGLInternalPixelFormat(format, result);
		CHECK_RESULT(result);

		CHECK_RESULT(SelectRenderbuffer(this));

		glRenderbufferStorage(GL_RENDERBUFFER, _format, size.x, size.y);		
		

		return Result();
	}
	Renderbuffer& Renderbuffer::operator=(Renderbuffer&& rb) noexcept
	{
		if (id != -1)
		{
			glDeleteRenderbuffers(1, &id);
			if (FlushOpenGLResult())
				return *this;

		}
		id = rb.id;
		rb.id = -1;
		return *this;
	}
}