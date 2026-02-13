#pragma once
#include "BlazeEngine/Graphics/Renderers/RendererBase.h"
#include "BlazeEngine/Graphics/Core/GraphicsAPI_OpenGL.h"

namespace Blaze::Graphics::OpenGL
{
	class BufferedRendererBase_OpenGL
	{
	public:
		BufferedRendererBase_OpenGL(uintMem bufferSize, GraphicsContext_OpenGL& graphicsContext);
		~BufferedRendererBase_OpenGL();

		void SetShaderProgram(Blaze::Graphics::OpenGL::ShaderProgram&& program);
	protected:
		ShaderProgram program;

		void FlushBuffer(uintMem offset, uintMem size);

		const auto& GetBuffer() const { return buffer; }
		const uintMem GetBufferSize() const { return bufferSize; }
		void* GetBufferMap() const { return bufferMap; }
	private:
		ImmutableMappedGraphicsBuffer buffer;
		uintMem bufferSize;
		void* bufferMap;
	};
}