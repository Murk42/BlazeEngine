#pragma once
#include "../Graphics3/OpenGL/API_OpenGL.h"
#include "TextRenderer_Dynamic.h"

namespace Graphics3::OpenGL
{
	class TextRenderer_OpenGL : public TextRendererBase
	{
		//GraphicsLibrary::ShaderProgram program;
		//GraphicsLibrary::ImmutableStaticGraphicsBuffer vb;
		//GraphicsLibrary::VertexArray va;
	public:
		TextRenderer_OpenGL(GraphicsContext& graphicsContext, RenderContext& transferRenderContext);

		void RenderText(Dynamic::RenderContextBase& context, Dynamic::FramebufferBase& target, TextRenderData renderData, std::initializer_list<Dynamic::Semaphore*> waitSemaphores, std::initializer_list<Dynamic::Semaphore*> signalSemaphores) override
		{
			Array<Semaphore*> _waitSemaphores { (uint)waitSemaphores.size() };
			Array<Semaphore*> _signalSemaphores { (uint)signalSemaphores.size() };
			for (uint i = 0; i < (uint)waitSemaphores.size(); ++i) _waitSemaphores[i] = (Semaphore*)waitSemaphores.begin()[i]->GetPtr();
			for (uint i = 0; i < (uint)signalSemaphores.size(); ++i) _signalSemaphores[i] = (Semaphore*)signalSemaphores.begin()[i]->GetPtr();
			RenderText((RenderContext&)context, (Framebuffer&)target, renderData,
				std::initializer_list<Semaphore*>(_waitSemaphores.Ptr(), _waitSemaphores.Ptr() + _waitSemaphores.Count()),
				std::initializer_list<Semaphore*>(_signalSemaphores.Ptr(), _signalSemaphores.Ptr() + _signalSemaphores.Count())
			);
		}
		void RenderText(RenderContext& context, Framebuffer& target, TextRenderData renderData, std::initializer_list<Semaphore*> waitSemaphores, std::initializer_list<Semaphore*> signalSemaphores);
	};	

	using TextRenderer = TextRenderer_OpenGL;
}