#pragma once
#include "../Graphics3/OpenGL/API_OpenGL.h"
#include "QuadRenderer_Dynamic.h"

namespace Graphics3::OpenGL
{
	class QuadRenderer_OpenGL : public QuadRendererBase
	{
		//GraphicsLibrary::ShaderProgram program;
		//GraphicsLibrary::ImmutableStaticGraphicsBuffer vb;
		//GraphicsLibrary::VertexArray va;
	public:
		QuadRenderer_OpenGL(GraphicsContext& graphicsContext, RenderContext& transferRenderContext);

		void RenderQuad(Dynamic::RenderContextBase& context, Dynamic::FramebufferBase& target, std::initializer_list<Dynamic::Semaphore*> waitSemaphores, std::initializer_list<Dynamic::Semaphore*> signalSemaphores) override
		{
			Array<Semaphore*> _waitSemaphores { (uint)waitSemaphores.size() };
			Array<Semaphore*> _signalSemaphores { (uint)signalSemaphores.size() };
			for (uint i = 0; i < (uint)waitSemaphores.size(); ++i) _waitSemaphores[i] = (Semaphore*)waitSemaphores.begin()[i]->GetPtr();
			for (uint i = 0; i < (uint)signalSemaphores.size(); ++i) _signalSemaphores[i] = (Semaphore*)signalSemaphores.begin()[i]->GetPtr();
			RenderQuad((RenderContext&)context, (Framebuffer&)target,
				std::initializer_list<Semaphore*>(_waitSemaphores.Ptr(), _waitSemaphores.Ptr() + _waitSemaphores.Count()),
				std::initializer_list<Semaphore*>(_signalSemaphores.Ptr(), _signalSemaphores.Ptr() + _signalSemaphores.Count())
			);
		}
		void RenderQuad(RenderContext& context, Framebuffer& target, std::initializer_list<Semaphore*> waitSemaphores, std::initializer_list<Semaphore*> signalSemaphores);
	};	

	using QuadRenderer = QuadRenderer_OpenGL;
}