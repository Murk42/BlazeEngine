#pragma once
#include "BlazeEngineCore/DataStructures/Color.h"
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineGraphics/Core/OpenGL/GraphicsContext_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLVertexArray.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLProgram.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLGraphicsBuffer.h"
#include "BlazeEngineGraphics/RenderStructure/StreamRenderer.h"

namespace Blaze::Graphics::OpenGL
{
	struct Line3DRenderData_OpenGL
	{
		ColorRGBAf color;
		Vec3f p1;
		Vec3f p2;
		float width;
	};	

	class BLAZE_GRAPHICS_API Line3DRenderer_OpenGL :
		public StreamRenderer
	{
	public:
		Line3DRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext, uintMem bufferLineCapacity = 64);

		void SetViewMatrix(const Mat4f& matrix);
		void SetProjectionMatrix(const Mat4f& matrix);
				
		void Render(RenderStream& stream, Vec2u targetSize) override;

		StringView GetRendererName() const override { return "Line3DRenderer"; }
	private:
		GraphicsContext_OpenGL& graphicsContext;
		const uintMem bufferInstanceCapacity;
		Blaze::Graphics::OpenGLWrapper::ShaderProgram program;
		Blaze::Graphics::OpenGLWrapper::ImmutableMappedGraphicsBuffer instanceBuffer;		
		Blaze::Graphics::OpenGLWrapper::VertexArray vertexArray;
		Mat4f viewMatrix, projectionMatrix;
	};
}