#include "pch.h"
#include "BlazeEngine/Graphics/Renderers/OpenGL/BufferedRendererBase_OpenGL.h"

namespace Blaze::Graphics::OpenGL
{
	BufferedRendererBase_OpenGL::BufferedRendererBase_OpenGL(uintMem bufferSize, GraphicsContext_OpenGL& graphicsContext)
		: program(0), bufferSize(bufferSize)
	{
		buffer.Allocate(nullptr, bufferSize, GraphicsBufferMapAccessFlags::Write, GraphicsBufferMapType::PersistentUncoherent);
		bufferMap = buffer.MapBufferRange(0, bufferSize, GraphicsBufferMapOptions::InvalidateBuffer | GraphicsBufferMapOptions::ExplicitFlush);

		if (bufferMap == nullptr)
			BLAZE_LOG_ERROR("Failed to map instance buffer");
	}
	BufferedRendererBase_OpenGL::~BufferedRendererBase_OpenGL()
	{
		buffer.UnmapBuffer();
	}
	void BufferedRendererBase_OpenGL::SetShaderProgram(Blaze::Graphics::OpenGL::ShaderProgram&& program)
	{
		this->program = std::move(program);
	}
	void BufferedRendererBase_OpenGL::FlushBuffer(uintMem offset, uintMem size)
	{
		buffer.FlushBufferRange(offset, size);
	}
}