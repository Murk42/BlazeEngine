#include "pch.h"
#include "BlazeEngine/Graphics/Core/OpenGL/FramebufferBase_OpenGL.h"

namespace Blaze::Graphics::OpenGL
{
	FramebufferBase_OpenGL::FramebufferBase_OpenGL(FramebufferBase_OpenGL&& other) noexcept
		: id(other.id)
	{
		other.id = 0;
	}
	FramebufferBase_OpenGL::FramebufferBase_OpenGL(uint32 id) 
		: id(id) 
	{
	}
	void FramebufferBase_OpenGL::ClearColor(Vec4i color, uint drawBuffer)
	{
		const GLint values[4]{
			static_cast<GLint>(color.x),
			static_cast<GLint>(color.y),
			static_cast<GLint>(color.z),
			static_cast<GLint>(color.w)
		};

		glClearNamedFramebufferiv(id, GL_COLOR, drawBuffer, values);
	}
	void FramebufferBase_OpenGL::ClearColor(Vec4u color, uint drawBuffer)
	{
		const GLuint values[4]{
			static_cast<GLuint>(color.x),
			static_cast<GLuint>(color.y),
			static_cast<GLuint>(color.z),
			static_cast<GLuint>(color.w)
		};

		glClearNamedFramebufferuiv(id, GL_COLOR, drawBuffer, values);
	}
	void FramebufferBase_OpenGL::ClearColor(Vec4f color, uint drawBuffer)
	{
		const GLfloat values[4]{
			static_cast<GLfloat>(color.x),
			static_cast<GLfloat>(color.y),
			static_cast<GLfloat>(color.z),
			static_cast<GLfloat>(color.w)
		};

		glClearNamedFramebufferfv(id, GL_COLOR, drawBuffer, values);
	}
	void FramebufferBase_OpenGL::ClearDepth(float value)
	{
		const GLfloat values[1]{
			static_cast<GLfloat>(value),
		};

		glClearNamedFramebufferfv(id, GL_DEPTH, 0, values);
	}
	void FramebufferBase_OpenGL::ClearStencil(int value)
	{
		const GLint values[1]{
			static_cast<GLint>(value),
		};

		glClearNamedFramebufferiv(id, GL_STENCIL, 0, values);
	}
	void FramebufferBase_OpenGL::ClearDepthStencil(float depthValue, int stencilValue)
	{
		glClearNamedFramebufferfi(id, GL_DEPTH_STENCIL, 0, static_cast<GLfloat>(depthValue), static_cast<GLint>(stencilValue));
	}
	FramebufferBase_OpenGL& FramebufferBase_OpenGL::operator=(FramebufferBase_OpenGL&& other) noexcept
	{
		id = other.id;
		other.id = 0;

		return *this;
	}
}