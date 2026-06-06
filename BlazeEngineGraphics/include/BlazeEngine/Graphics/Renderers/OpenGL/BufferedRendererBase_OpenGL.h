#pragma once
#include "BlazeEngine/Graphics/Core/OpenGL/RendererBase_OpenGL.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLProgram.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLGraphicsBuffer.h"

namespace Blaze::Graphics::OpenGL
{
	class BLAZE_API BufferedRendererBase_OpenGL : public RendererBase_OpenGL
	{
	public:
		BufferedRendererBase_OpenGL(GraphicsContext_OpenGL& graphicsContext, uintMem bufferSize);
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