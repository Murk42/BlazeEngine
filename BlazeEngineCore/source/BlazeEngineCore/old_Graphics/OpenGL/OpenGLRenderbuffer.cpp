#include "BlazeEngine/Graphics/OpenGL/OpenGLRenderbuffer.h"
#include "BlazeEngine/Internal/Conversions.h"

#include "GL/glew.h"

#include "BlazeEngine/Graphics/GraphicsCore.h"

namespace Blaze::OpenGL
{
	Renderbuffer::Renderbuffer()		
		: id(-1)
	{
		glGenRenderbuffers(1, &id);		

		if (FlushOpenGLResult())
			return;
		
		Graphics::Renderer::SelectRenderbuffer(this);

		glBindRenderbuffer(GL_RENDERBUFFER, id);		
		FlushOpenGLResult();
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
			FlushOpenGLResult();
		}
	}
	Result OpenGL::Renderbuffer::Create(Vec2i size, Blaze::OpenGL::TextureInternalPixelFormat format)
	{
		this->size = size;

		Result result;
		GLenum _format = OpenGLInternalPixelFormat(format, result);
		CHECK_RESULT(result);

		CHECK_RESULT(Graphics::Renderer::SelectRenderbuffer(this));

		glRenderbufferStorage(GL_RENDERBUFFER, _format, size.x, size.y);		
		FlushOpenGLResult();

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