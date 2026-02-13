#include "pch.h"
#include "BlazeEngine/Graphics/Renderers/OpenGL/BasicIndexedMeshRenderer_OpenGL.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLShader.h"
#include "BlazeEngine/Graphics/Shaders/Shaders.h"
#include "BlazeEngine/Core/Common/Buffer.h"

namespace Blaze::Graphics::OpenGL
{
	struct Vertex
	{
		Vec3f pos;
		uint32 meshIndex;
	};
	struct MeshData
	{
		Mat4f transformMatrix;
		ColorRGBAf color;
	};

	constexpr uintMem BatchMaxMeshCount = 128;
	constexpr uintMem BatchBufferSize = sizeof(Vertex) * 1024 + sizeof(uint32) * 4 * 1024;

	BasicIndexedMeshRenderer_OpenGL::BasicIndexedMeshRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext, bool loadDefaultShaders)
		: BufferedRendererBase_OpenGL(BatchBufferSize, graphicsContext), graphicsContext(graphicsContext), vertexCount(0), indexCount(0), meshCount(0), rendering(false)
	{
		if (loadDefaultShaders)
		{
			auto vert = OpenGL::VertexShader(ShaderSources::basicMesh_vert);
			auto frag = OpenGL::FragmentShader(ShaderSources::basicMesh_frag);
			auto geom = OpenGL::GeometryShader(ShaderSources::basicMesh_geom);
			SetShaderProgram(ShaderProgram({vert, frag, geom}));
		}

		va.EnableVertexAttribute(0);
		va.SetFloatVertexAttributeFormat(0, FloatVertexAttributeType::Float, 3, offsetof(Vertex, pos));
		va.SetVertexAttributeBuffer(0, &GetBuffer(), sizeof(Vertex), 0);
		va.EnableVertexAttribute(1);
		va.SetIntegerVertexAttributeFormat(1, IntegerVertexAttributeType::Uint32, 1, offsetof(Vertex, meshIndex));
		va.SetVertexAttributeBuffer(1, &GetBuffer(), sizeof(Vertex), 0);

		va.SetIndexBuffer(GetBuffer());

		meshDataBuffer.Allocate(nullptr, sizeof(MeshData) * BatchMaxMeshCount, GraphicsBufferMapAccessFlags::Write, GraphicsBufferMapType::PersistentUncoherent);
		meshDataMap = meshDataBuffer.MapBufferRange(0, sizeof(MeshData) * BatchMaxMeshCount, GraphicsBufferMapOptions::InvalidateBuffer | GraphicsBufferMapOptions::ExplicitFlush);
	}
	BasicIndexedMeshRenderer_OpenGL::~BasicIndexedMeshRenderer_OpenGL()
	{
		WaitFence();
		meshDataBuffer.UnmapBuffer();
	}
	void BasicIndexedMeshRenderer_OpenGL::StartRender(const RenderContext& context)
	{
		graphicsContext.SelectVertexArray(&va);
		graphicsContext.SelectProgram(&program);
		graphicsContext.BindUniformBuffer(meshDataBuffer, 0);
		graphicsContext.SetShaderUniformBlockBindingPoint(program, 0, 0);
		rendering = true;
	}
	void BasicIndexedMeshRenderer_OpenGL::EndRender(const RenderContext& context)
	{
		Flush(context);
		rendering = false;
	}
	void BasicIndexedMeshRenderer_OpenGL::SetProjectionMatrix(const Mat4f& matrix)
	{
		if (rendering)
		{
			BLAZE_LOG_ERROR("Cannot change uniforms while rendering");
			return;
		}

		program.SetUniform(0, matrix);
	}
	void BasicIndexedMeshRenderer_OpenGL::SetViewMatrix(const Mat4f& matrix)
	{
		if (rendering)
		{
			BLAZE_LOG_ERROR("Cannot change uniforms while rendering");
			return;
		}

		program.SetUniform(1, matrix);
	}
	void BasicIndexedMeshRenderer_OpenGL::SetShadingOptions(Vec3f lightDirection, ColorRGBf ambientColor)
	{
		if (rendering)
		{
			BLAZE_LOG_ERROR("Cannot change uniforms while rendering");
			return;
		}

		program.SetUniform(2, lightDirection);
		program.SetUniform(3, (Vec3f)ambientColor);
	}
	void BasicIndexedMeshRenderer_OpenGL::Render(ArrayView<Vec3f> vertices, ArrayView<uint32> indices, Mat4f modelMatrix, ColorRGBAf color, const RenderContext& context)
	{
		if (!rendering)
		{
			BLAZE_LOG_ERROR("Render called outside of a rendering session");
			return;
		}

		if (indices.Empty())
			return;

		if (indices.Count() % 3 != 0)
		{
			BLAZE_LOG_ERROR("The number of indices must be a multiple of 3");
			return;
		}

		if (meshCount == BatchMaxMeshCount)
			Flush(context);

		WaitFence();

		uintMem availableSize = GetBufferSize() - (vertexCount * sizeof(Vertex) + indexCount * sizeof(uint32));
		uintMem requiredSize = vertices.Count() * sizeof(Vertex) + indices.Count() * sizeof(uint32);

		Vertex* vertexBuffer = reinterpret_cast<Vertex*>(GetBufferMap());
		uint32* indexBuffer = reinterpret_cast<uint32*>(reinterpret_cast<byte*>(GetBufferMap()) + GetBufferSize());

		if (availableSize < requiredSize)
		{
			uintMem srcIndexBufferIndex = 0;
			while (true)
			{
				while ((vertexCount + 3) * sizeof(Vertex) + (indexCount + 3) * sizeof(uint32) <= GetBufferSize())
				{
					uint32 i1 = indices[srcIndexBufferIndex + 0];
					uint32 i2 = indices[srcIndexBufferIndex + 1];
					uint32 i3 = indices[srcIndexBufferIndex + 2];

					vertexBuffer[vertexCount + 0] = Vertex{ vertices[i1], static_cast<uint32>(meshCount) };
					vertexBuffer[vertexCount + 1] = Vertex{ vertices[i2], static_cast<uint32>(meshCount) };
					vertexBuffer[vertexCount + 2] = Vertex{ vertices[i3], static_cast<uint32>(meshCount) };

					indexBuffer[-indexCount - 3] = vertexCount + 0;
					indexBuffer[-indexCount - 2] = vertexCount + 1;
					indexBuffer[-indexCount - 1] = vertexCount + 2;

					srcIndexBufferIndex += 3;
					indexCount += 3;
					vertexCount += 3;

					if (srcIndexBufferIndex == indices.Count())
						break;
				}

				if (srcIndexBufferIndex == indices.Count())
					break;

				reinterpret_cast<MeshData*>(meshDataMap)[meshCount++] = MeshData{
					.transformMatrix = modelMatrix.Transposed(),
					.color = color
				};

				Flush(context);

				WaitFence();
			}
		}
		else
			CopySubMesh(vertices, indices);

		reinterpret_cast<MeshData*>(meshDataMap)[meshCount++] = MeshData{
			.transformMatrix = modelMatrix.Transposed(),
			.color = color
		};
	}
	void BasicIndexedMeshRenderer_OpenGL::Flush(const RenderContext& context)
	{
		if (indexCount == 0)
			return;

		uintMem indicesOffset = GetBufferSize() - sizeof(uint32) * indexCount;
		FlushBuffer(0, sizeof(Vertex) * vertexCount);
		FlushBuffer(indicesOffset, sizeof(uint32) * indexCount);

		meshDataBuffer.FlushBufferRange(0, sizeof(MeshData) * meshCount);

		graphicsContext.RenderIndexedPrimitives(PrimitiveType::Triangles, IndexType::Uint32, indexCount, indicesOffset);
		fence.SetFence();

		vertexCount = 0;
		indexCount = 0;
		meshCount = 0;

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
	void BasicIndexedMeshRenderer_OpenGL::CopySubMesh(ArrayView<Vec3f> vertices, ArrayView<uint32> indices)
	{
		void* buffer = GetBufferMap();

		Vertex* vertexBuffer = reinterpret_cast<Vertex*>(buffer) + vertexCount;
		for (uintMem i = 0; i < vertices.Count(); ++i)
		{
			vertexBuffer[i].pos = vertices[i];
			vertexBuffer[i].meshIndex = static_cast<uint32>(meshCount);
		}

		uint32* indexBuffer = reinterpret_cast<uint32*>(reinterpret_cast<byte*>(buffer) + GetBufferSize()) - indexCount - indices.Count();
		for (uintMem i = 0; i < indices.Count(); ++i)
		{
			indexBuffer[i] = indices[i] + static_cast<uint32>(vertexCount);
		}

		vertexCount += vertices.Count();
		indexCount += indices.Count();
	}
}
