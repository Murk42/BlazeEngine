#include "pch.h"
#include "BlazeEngineCore/File/File.h"
#include "BlazeEngineCore/DataStructures/Buffer.h"
#include "BlazeEngineGraphics/Renderers/OpenGL/BasicMeshRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLShader.h"
#include "BlazeEngineGraphics/Shaders/Shaders.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace Blaze
{
	IndexedTriangleMesh::IndexedTriangleMesh()
	{
	}
	void IndexedTriangleMesh::Clear()
	{
		vertices.Clear();
		indicies.Clear();
	}
	bool IndexedTriangleMesh::Load(const Path& path, StringView objectName)
	{
		File file{ path, FileAccessPermission::Read };
		return Load(file, path.FileExtension());
	}
	bool IndexedTriangleMesh::Load(ReadStream& stream, StringViewUTF8 fileExtensionHint, StringView objectName)
	{
		Assimp::Importer importer;
		Buffer streamBuffer{ stream.GetSize() };
		stream.Read(streamBuffer.Ptr(), stream.GetSize());

		auto scene = importer.ReadFileFromMemory(streamBuffer.Ptr(), stream.GetSize(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices, (const char*)fileExtensionHint.Buffer());

		if (scene == nullptr)
		{
			BLAZE_ENGINE_ERROR("Failed to loading a basic triangle mesh. Assimp returned error string: {}", importer.GetErrorString());
			return false;
		}

		if (scene->mRootNode->mNumChildren == 0)
		{
			BLAZE_ENGINE_ERROR("Loading a basic triangle mesh from a scene with no objects");
			return false;
		}

		aiNode* node = nullptr;
		if (objectName.Empty())
			if (scene->mRootNode->mNumChildren == 1)
				node = scene->mRootNode->mChildren[0];
			else
			{
				BLAZE_ENGINE_ERROR("Loading a basic triangle mesh with no object name from a scene with more than one object");
				return false;
			}
		else
		{
			node = scene->mRootNode->FindNode(objectName.Ptr());
			if (node == nullptr)
			{
				BLAZE_ENGINE_ERROR("Failed to find a object named \"{}\" while loading a basic triangle mesh", objectName);
				return false;
			}
		}

		uintMem indexCount = 0;
		uintMem vertexCount = 0;
		for (uintMem i = 0; i < node->mNumMeshes; ++i)
		{
			auto mesh = scene->mMeshes[node->mMeshes[i]];
			indexCount += mesh->mNumFaces * 3;
			vertexCount += mesh->mNumVertices;
		}

		indicies.ReserveExactly(indexCount);
		vertices.ReserveExactly(vertexCount);

		for (uintMem i = 0; i < node->mNumMeshes; ++i)
		{
			auto mesh = scene->mMeshes[node->mMeshes[i]];

			for (uintMem j = 0; j < mesh->mNumFaces; ++j)
			{
				indicies.AddBack(mesh->mFaces[j].mIndices[0]);
				indicies.AddBack(mesh->mFaces[j].mIndices[1]);
				indicies.AddBack(mesh->mFaces[j].mIndices[2]);
			}

			for (uintMem j = 0; j < mesh->mNumVertices; ++j)
				vertices.AddBack(Vec3f(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z));
		}
	}
	void IndexedTriangleMesh::Load(Array<Vec3f>&& vertices, Array<uint32>&& indicies)
	{
		this->vertices = std::move(vertices);
		this->indicies = std::move(indicies);
	}
	Array<IndexedTriangleMesh::Triangle> IndexedTriangleMesh::CreateTriangleArray() const
	{
		Array<Triangle> triangles;
		triangles.ReserveExactly(indicies.Count() / 3);

		for (uintMem i = 0; i < triangles.ReservedCount(); ++i)
		{
			Triangle t;
			t.p1 = vertices[indicies[i * 3 + 0]];
			t.p2 = vertices[indicies[i * 3 + 1]];
			t.p3 = vertices[indicies[i * 3 + 2]];

			triangles.AddBack(t);
		}

		return triangles;
	}
}
namespace Blaze::Graphics::OpenGL
{	

	BasicMeshRenderData_OpenGL::BasicMeshRenderData_OpenGL()
		: indexCount(0), buffer(0)
	{
		using namespace OpenGLWrapper;
		va.EnableVertexAttribute(0);
		va.SetFloatVertexAttributeFormat(0, FloatVertexAttributeType::Float, 3, 0);
	}
	void BasicMeshRenderData_OpenGL::Clear()
	{
		indexCount = 0;
		buffer.Release();
		va.Release();
	}
	void BasicMeshRenderData_OpenGL::Initialize(const IndexedTriangleMesh& mesh)
	{
		using namespace OpenGLWrapper;

		auto vertices = mesh.GetVertices();
		auto indicies = mesh.GetIndicies();

		uintMem bufferSize = indicies.Count() * sizeof(uint32) + vertices.Count() * sizeof(Vec3f);
		Buffer packedBuffer{ bufferSize };
		memcpy(packedBuffer.Ptr(), indicies.Ptr(), indicies.Count() * sizeof(uint32));
		memcpy((char*)packedBuffer.Ptr() + indicies.Count() * sizeof(uint32), vertices.Ptr(), vertices.Count() * sizeof(Vec3f));

		buffer = ImmutableStaticGraphicsBuffer();
		buffer.Allocate(packedBuffer.Ptr(), bufferSize);
		va.SetIndexBuffer(buffer);
		va.SetVertexAttributeBuffer(0, &buffer, sizeof(Vec3f), indicies.Count() * sizeof(uint32));

		indexCount = indicies.Count();
	}

	BasicMeshRenderer_OpenGL::BasicMeshRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext)
		: graphicsContext(graphicsContext)
	{			
		auto vert = OpenGLWrapper::VertexShader(ShaderSources::basicMesh_vert);
		auto frag = OpenGLWrapper::FragmentShader(ShaderSources::basicMesh_frag);
		auto geom = OpenGLWrapper::GeometryShader(ShaderSources::basicMesh_geom);
		program.LinkShaders({ &vert, &frag, &geom });
	}
	void BasicMeshRenderer_OpenGL::SetProjectionMatrix(const Mat4f& matrix)
	{
		program.SetUniform(0, matrix);
	}
	void BasicMeshRenderer_OpenGL::SetViewMatrix(const Mat4f& matrix)
	{
		program.SetUniform(1, matrix);
	}
	void BasicMeshRenderer_OpenGL::SetShadingOptions(Vec3f lightDirection, ColorRGBf ambientColor)
	{
		program.SetUniform(3, lightDirection);
		program.SetUniform(4, (Vec3f)ambientColor);
	}
	void BasicMeshRenderer_OpenGL::Render(BasicMeshRenderData_OpenGL& renderData, const Mat4f& modelMatrix, ColorRGBAf color)
	{
		program.SetUniform(2, modelMatrix);
		program.SetUniform(5, color);

		graphicsContext.SelectVertexArray(&renderData.GetVertexArray());
		graphicsContext.SelectProgram(&program);

		graphicsContext.RenderIndexedPrimitives(OpenGLWrapper::PrimitiveType::Triangles, OpenGLWrapper::IndexType::Uint32, renderData.GetIndexCount(), 0);
	}
}
