#include "BlazeEngine/Graphics/OpenGL/OpenGLRenderbuffer.h"
#include "source/BlazeEngine/Internal/Conversions.h"

#include "GL/glew.h"

#include "BlazeEngine/Graphics/GraphicsCore.h"

namespace Blaze::OpenGL
{
	Renderbuffer::Renderbuffer()		
		: id(-1)
	{
		glGenRenderbuffers(1, &id);		

		if (FLUSH_OPENGL_RESULT())
			return;
		
		ADD_STACK_FRAME(Graphics::Core::SelectRenderbuffer(this));		

		glBindRenderbuffer(GL_RENDERBUFFER, id);		
		FLUSH_OPENGL_RESULT();
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
			FLUSH_OPENGL_RESULT();
		}
	}
	Result OpenGL::Renderbuffer::Create(Vec2i size, Blaze::OpenGL::TextureInternalPixelFormat format)
	{
		this->size = size;

		Result result;
		GLenum _format = OpenGLInternalPixelFormat(format, result);
		CHECK_RESULT(result);

		CHECK_RESULT(Graphics::Core::SelectRenderbuffer(this));

		glRenderbufferStorage(GL_RENDERBUFFER, _format, size.x, size.y);		
		FLUSH_OPENGL_RESULT();

		return Result();
	}
	Renderbuffer& Renderbuffer::operator=(Renderbuffer&& rb) noexcept
	{
		if (id != -1)
		{
			glDeleteRenderbuffers(1, &id);
			if (FLUSH_OPENGL_RESULT())
				return *this;

		}
		id = rb.id;
		rb.id = -1;
		return *this;
	}
}