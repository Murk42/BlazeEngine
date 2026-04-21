#include "pch.h"
#include "BlazeEngine/Graphics/Core/OpenGL/RenderWindowFramebuffer_OpenGL.h"
#include "BlazeEngine/Graphics/Core/OpenGL/RenderWindow_OpenGL.h"

#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLConversions.h"

namespace Blaze::Graphics::OpenGL
{	
	RenderWindowFramebuffer_OpenGL::~RenderWindowFramebuffer_OpenGL()
	{
	}
	void RenderWindowFramebuffer_OpenGL::SetDrawBuffer(RenderWindowFramebufferBufferTargets target)
	{
		GLenum value;
		if (!OpenGLRenderWindowFramebufferBufferTargets(target, value))
		{
			BLAZE_LOG_ERROR("Invalid RenderWindowFramebufferBufferTargets enum value");
			return;
		}

		glNamedFramebufferDrawBuffer(id, value);
	}
	void RenderWindowFramebuffer_OpenGL::SetDrawBuffers(const ArrayView<RenderWindowFramebufferDrawBufferInfo>& targets)
	{
		if (targets.Empty())
		{
			glNamedFramebufferDrawBuffers(id, 0, nullptr);
			return;
		}

		uint maxDrawBufferIndex = 0;
		for (auto& target : targets)
			maxDrawBufferIndex = std::max(maxDrawBufferIndex, target.drawBufferIndex);

		Array<GLenum> values;
		values.Resize(static_cast<uintMem>(maxDrawBufferIndex) + 1, GL_NONE);

		for (auto& target : targets)
		{
			GLenum value;
			if (!OpenGLRenderWindowFramebufferBufferTarget(target.target, value))
			{
				BLAZE_LOG_ERROR("Invalid RenderWindowFramebufferBufferTarget enum value");
				return;
			}

			values[target.drawBufferIndex] = value;
		}

		glNamedFramebufferDrawBuffers(id, values.Count(), values.Ptr());
	}
	void RenderWindowFramebuffer_OpenGL::DisableDrawBuffers()
	{
		glNamedFramebufferDrawBuffer(id, GL_NONE);
	}
	RenderWindowFramebuffer_OpenGL::RenderWindowFramebuffer_OpenGL(uint32 id, Vec2u size)
		: FramebufferBase_OpenGL(id), size(size)
	{
	}	
}