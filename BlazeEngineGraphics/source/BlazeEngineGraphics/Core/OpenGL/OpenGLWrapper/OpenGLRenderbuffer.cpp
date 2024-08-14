#include "pch.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLRenderbuffer.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLConversions.h"

namespace Blaze::Graphics::OpenGLWrapper
{
	Renderbuffer::Renderbuffer()		
		: id(0)
	{
		glGenRenderbuffers(1, &id);	
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
	void Renderbuffer::Create(Vec2i size, Blaze::Graphics::OpenGLWrapper::TextureInternalPixelFormat format)
	{
		this->size = size;

		Result result;
		GLenum _format = OpenGLInternalPixelFormat(format, result);
		if (result) return;
		
		glNamedRenderbufferStorage(id, _format, size.x, size.y);				
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