#include "pch.h"
#include "BlazeEngineGraphics/Renderers/OpenGL/Line3DRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLShader.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLFence.h"
#include "BlazeEngineGraphics/Files/shaders.h"

namespace Blaze::Graphics::OpenGL
{
	struct Line3DInstance
	{
		ColorRGBAf color;
		Vec3f p1;
		Vec3f p2;
		float width;
	};
	 
	Line3DRenderer_OpenGL::Line3DRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext, uintMem bufferLineCapacity)
		: graphicsContext(graphicsContext), bufferInstanceCapacity(bufferLineCapacity)
	{
		Blaze::Graphics::OpenGLWrapper::VertexShader vert;// { "assets/shaders/OpenGL/line3d.vert" };
		vert.ShaderSource(StringView((const char*)line3d_vert_file, line3d_vert_size));
		vert.CompileShader();

		Blaze::Graphics::OpenGLWrapper::FragmentShader frag;// { "assets/shaders/OpenGL/line3d.frag" };
		frag.ShaderSource(StringView((const char*)line3d_frag_file, line3d_frag_size));
		frag.CompileShader();

		program.LinkShaders({ &vert, &frag });

		instanceBuffer.Allocate(nullptr, sizeof(Line3DInstance) * bufferLineCapacity,
			OpenGLWrapper::ImmutableGraphicsBufferMapAccess::Write,
			OpenGLWrapper::ImmutableGraphicsBufferMapType::PersistentUncoherent
		);		

		vertexArray.EnableVertexAttribute(0);
		vertexArray.SetVertexAttributeFormat(0, OpenGLWrapper::VertexAttributeType::Float, 4, false, offsetof(Line3DInstance, color));
		vertexArray.SetVertexAttributeBuffer(0, &instanceBuffer, sizeof(Line3DInstance), 0);
		vertexArray.SetVertexAttributeDivisor(0, 1);
		vertexArray.EnableVertexAttribute(1);
		vertexArray.SetVertexAttributeFormat(1, OpenGLWrapper::VertexAttributeType::Float, 3, false, offsetof(Line3DInstance, p1));
		vertexArray.SetVertexAttributeBuffer(1, &instanceBuffer, sizeof(Line3DInstance), 0);
		vertexArray.SetVertexAttributeDivisor(1, 1);
		vertexArray.EnableVertexAttribute(2);
		vertexArray.SetVertexAttributeFormat(2, OpenGLWrapper::VertexAttributeType::Float, 3, false, offsetof(Line3DInstance, p2));
		vertexArray.SetVertexAttributeBuffer(2, &instanceBuffer, sizeof(Line3DInstance), 0);
		vertexArray.SetVertexAttributeDivisor(2, 1);
		vertexArray.EnableVertexAttribute(3);
		vertexArray.SetVertexAttributeFormat(3, OpenGLWrapper::VertexAttributeType::Float, 1, false, offsetof(Line3DInstance, width));
		vertexArray.SetVertexAttributeBuffer(3, &instanceBuffer, sizeof(Line3DInstance), 0);
		vertexArray.SetVertexAttributeDivisor(3, 1);
	}

	void Line3DRenderer_OpenGL::SetViewMatrix(const Mat4f& matrix)
	{		
		viewMatrix = matrix;  
	}

	void Line3DRenderer_OpenGL::SetProjectionMatrix(const Mat4f& matrix)
	{
		projectionMatrix = matrix;	 	
	}

	void Line3DRenderer_OpenGL::Render(RenderStream& renderStream, Vec2u targetSize)
	{		
		graphicsContext.SelectProgram(&program);
		graphicsContext.SelectVertexArray(&vertexArray);						

		program.SetUniform(0, projectionMatrix * viewMatrix);
		program.SetUniform(1, projectionMatrix);
		program.SetUniform(2, (Vec2f)targetSize);

		OpenGLWrapper::Fence fence{ };
		fence.SetFence();		

		renderStream.BeginStream();

		Line3DRenderData_OpenGL* rd = nullptr;

		while (true)
		{
			if (fence.BlockClient(1.0) == Blaze::Graphics::OpenGLWrapper::FenceReturnState::TimeoutExpired)
			{
				Debug::Logger::LogWarning("Blaze Graphics API", "Fence timeout");
				return;
			}
			
			Line3DInstance* instanceBufferMap = (Line3DInstance*)instanceBuffer.MapBufferRange(0, sizeof(Line3DInstance) * bufferInstanceCapacity, Blaze::Graphics::OpenGLWrapper::ImmutableGraphicsBufferMapOptions::ExplicitFlush);
			uintMem instanceCount = 0;

			while (true)				
			{
				rd = (Line3DRenderData_OpenGL*)renderStream.Advance();

				if (rd == nullptr)
					break;				

				Line3DInstance instance = {
					.color = rd->color,
					.p1 = rd->p1,
					.p2 = rd->p2,
					.width = rd->width
				};

				instanceBufferMap[instanceCount] = instance;
				++instanceCount;

				if (instanceCount == bufferInstanceCapacity)
					break;
			}

			if (instanceCount == 0)							
				break;			

			instanceBuffer.FlushBufferRange(0, sizeof(Line3DInstance) * instanceCount);						
			instanceBuffer.UnmapBuffer();

			graphicsContext.RenderInstancedPrimitiveArray(Blaze::Graphics::OpenGLWrapper::PrimitiveType::TriangleStrip, 0, 4, 0, instanceCount);

			fence.SetFence();			

			if (rd == nullptr)
				break;
		}
	}
}