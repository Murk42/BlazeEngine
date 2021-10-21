#include "BlazeEngine/Graphics/OpenGL/OpenGLRenderbuffer.h"
#include "source/BlazeEngine/Internal/Conversions.h"

#include "GL/glew.h"

namespace Blaze::OpenGL
{
	Renderbuffer::Renderbuffer()		
		: id(-1)
	{
		glGenRenderbuffers(1, &id);
	}
	Renderbuffer::Renderbuffer(Renderbuffer&& rb) noexcept
		: id(rb.id)
	{
		rb.id = -1;
	}
	Renderbuffer::~Renderbuffer()
	{
		if (id != -1)
			glDeleteRenderbuffers(1, &id);
	}
	void OpenGL::Renderbuffer::Create(Vec2i size, Blaze::OpenGL::TextureInternalPixelFormat format)
	{
		this->size = size;
		GLenum _format = OpenGLInternalPixelFormat(format);
		glNamedRenderbufferStorage(id, _format, size.x, size.y);
	}
	Renderbuffer& Renderbuffer::operator=(Renderbuffer&& rb) noexcept
	{
		if (id != -1)
			glDeleteRenderbuffers(1, &id);
		id = rb.id;
		rb.id = -1;
		return *this;
	}
}