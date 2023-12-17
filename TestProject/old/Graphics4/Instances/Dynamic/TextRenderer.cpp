#include "pch.h"
#include "TextRenderer.h"
#include "Graphics4/OpenGL/API_OpenGL.h"
#include "Graphics4/OpenGL/TextRenderer_OpenGL.h"

namespace Graphics4::Dynamic
{
	TextRendererBase::TextRendererBase(Graphics4::Dynamic::GraphicsContextBase& graphicsContext)
	{
		auto* graphicsContext_OpenGL = graphicsContext.TryCast<Graphics4::OpenGL::GraphicsContext_OpenGL>();

		if (graphicsContext_OpenGL != nullptr)
			SetImplementation<Graphics4::OpenGL::TextRenderer_OpenGL>(*graphicsContext_OpenGL);
		else
			Debug::Logger::LogError("Blaze Graphics API", "Unsupported implementation");
	}
	bool TextRendererBase::CreateTask(const TextRenderData& renderData, TextRenderTaskBase& task)
	{
		if (auto impl = GetImplementation())
			return impl->CreateTask(renderData, task);		

		return false;
	}
	TextRendererBase::TextRendererBase()
	{
	}
}