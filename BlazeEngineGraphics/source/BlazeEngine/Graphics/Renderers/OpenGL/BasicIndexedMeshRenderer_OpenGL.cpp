#include "pch.h"
#include "BlazeEngine/Graphics/Renderers/OpenGL/BasicIndexedMeshRenderer_OpenGL.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLShader.h"
#include "BlazeEngine/Graphics/Shaders/Shaders.h"
#include "BlazeEngine/Core/Common/Buffer.h"

namespace Blaze::Graphics::OpenGL
{
	BasicIndexedMeshRenderer_OpenGL::BasicIndexedMeshRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext, bool loadDefaultShaders)
		: graphicsContext(graphicsContext)
	{
		if (loadDefaultShaders)
		{
			auto vert = OpenGL::VertexShader(ShaderSources::basicMesh_vert);
			auto frag = OpenGL::FragmentShader(ShaderSources::basicMesh_frag);
			auto geom = OpenGL::GeometryShader(ShaderSources::basicMesh_geom);
			SetShaderProgram(ShaderProgram({&vert, &frag, &geom}));
		}

		va.EnableVertexAttribute(0);
	}
	BasicIndexedMeshRenderer_OpenGL::~BasicIndexedMeshRenderer_OpenGL()
	{
		WaitFence();
	}
	void BasicIndexedMeshRenderer_OpenGL::StartRender(const RenderContext& context)
	{
	}
	void BasicIndexedMeshRenderer_OpenGL::EndRender(const RenderContext& context)
	{
	}
	void BasicIndexedMeshRenderer_OpenGL::SetProjectionMatrix(const Mat4f& matrix)
	{
		program.SetUniform(0, matrix);
	}
	void BasicIndexedMeshRenderer_OpenGL::SetViewMatrix(const Mat4f& matrix)
	{
		program.SetUniform(1, matrix);
	}
	void BasicIndexedMeshRenderer_OpenGL::SetShadingOptions(Vec3f lightDirection, ColorRGBf ambientColor)
	{
		program.SetUniform(3, lightDirection);
		program.SetUniform(4, (Vec3f)ambientColor);
	}
	void BasicIndexedMeshRenderer_OpenGL::Render(ArrayView<Vec3f> vertices, ArrayView<uint32> indices, Mat4f modelMatrix, ColorRGBAf color, const RenderContext& context)
	{
		uintMem bufferSize = indices.Count() * sizeof(uint32) + vertices.Count() * sizeof(Vec3f);

		Buffer packedBuffer{ bufferSize };
		memcpy(packedBuffer.Ptr(), indices.Ptr(), indices.Count() * sizeof(uint32));
		memcpy((char*)packedBuffer.Ptr() + indices.Count() * sizeof(uint32), vertices.Ptr(), vertices.Count() * sizeof(Vec3f));

		WaitFence();

		buffer = ImmutableStaticGraphicsBuffer();
		buffer.Allocate(packedBuffer.Ptr(), bufferSize);

		va.SetIndexBuffer(buffer);
		va.SetVertexAttributeBuffer(0, &buffer, sizeof(Vec3f), indices.Count() * sizeof(uint32));

		program.SetUniform(2, modelMatrix);
		program.SetUniform(5, color);

		graphicsContext.SelectVertexArray(&va);
		graphicsContext.SelectProgram(&program);

		graphicsContext.RenderIndexedPrimitives(OpenGL::PrimitiveType::Triangles, OpenGL::IndexType::Uint32, indices.Count(), 0);
		fence.SetFence();
	}
	void BasicIndexedMeshRenderer_OpenGL::SetShaderProgram(Blaze::Graphics::OpenGL::ShaderProgram&& newProgram)
	{
		WaitFence();
		program = std::move(newProgram);

		program.SetUniform(3, Vec3f(-1, -1, -1).Normalised());
		program.SetUniform(4, Vec3f(1, 1, 1));
	}
	void BasicIndexedMeshRenderer_OpenGL::WaitFence()
	{
		switch (fence.BlockClient(2.0f))
		{
		case FenceReturnState::Error:
			BLAZE_LOG_ERROR("Error occured while waiting on a fence");
			break;
		default:
			BLAZE_LOG_ERROR("An invalid value was returned while waiting on a fence");
			break;
		case FenceReturnState::TimeoutExpired:
			BLAZE_LOG_ERROR("Timeout expired while waiting on a fence");
			break;
		case FenceReturnState::ConditionSatisfied:
		case FenceReturnState::AlreadySignaled:
		case FenceReturnState::FenceNotSet:
			break;
		}
	}
}
