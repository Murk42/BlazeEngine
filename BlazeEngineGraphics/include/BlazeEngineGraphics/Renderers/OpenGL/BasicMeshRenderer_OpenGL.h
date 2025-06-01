#pragma once
#include "BlazeEngineCore/DataStructures/Color.h"
#include "BlazeEngineGraphics/Core/OpenGL/GraphicsContext_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLProgram.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLVertexArray.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLGraphicsBuffer.h"

namespace Blaze
{
	class IndexedTriangleMesh
	{
	public:
		struct Triangle
		{
			Vec3f p1;
			Vec3f p2;
			Vec3f p3;
		};

		IndexedTriangleMesh();
		IndexedTriangleMesh(const IndexedTriangleMesh& other) = default;
		IndexedTriangleMesh(IndexedTriangleMesh&& other) noexcept = default;
		inline IndexedTriangleMesh(const Path& path, StringView objectName = StringView()) { Load(path, objectName); }
		inline IndexedTriangleMesh(Array<Vec3f>&& vertices, Array<uint32>&& indicies) { Load(std::move(vertices), std::move(indicies)); }

		void Clear();

		/*
			Loads a mesh of an object named 'objectName' from a file. If there is no object named that way or
			the file could not be loaded successfully false is returned, true otherwise. If 'objectName' is an
			empty string and the file contains one object then that objects mesh is loaded.

			\param path - path to the file from which to load the mesh
			\param objectName - object whose mesh to load. This can be an empty string in which case the mesh of
			the only object in the scene is loaded, and if there is more than one object the load fails.
			\returns true on success, false otherwise
		*/
		bool Load(const Path& path, StringView objectName = StringView());
		/*
			Loads a mesh of an object named 'objectName' from a stream. If there is no object named that way or
			the stream could not be loaded successfully false is returned, true otherwise. If 'objectName' is an
			empty string and the stream contains one object then that objects mesh is loaded.

			\param path - path to the file from which to load the mesh
			\param objectName - object whose mesh to load. This can be an empty string in which case the mesh of
			the only object in the scene is loaded, and if there is more than one object the load fails.
			\returns true on success, false otherwise
		*/
		bool Load(ReadStream& stream, StringViewUTF8 fileExtensionHint, StringView objectName = StringView());

		void Load(Array<Vec3f>&& vertices, Array<uint32>&& indicies);

		inline ArrayView<uint32> GetIndicies() const { return indicies; }
		inline ArrayView<Vec3f> GetVertices() const { return vertices; }
		Array<Triangle> CreateTriangleArray() const;

		IndexedTriangleMesh& operator=(const IndexedTriangleMesh& other) = default;
		IndexedTriangleMesh& operator=(IndexedTriangleMesh&& other) noexcept = default;
	private:
		Array<uint32> indicies;
		Array<Vec3f> vertices;
	};
}

namespace Blaze::Graphics::OpenGL
{

	class BasicMeshRenderData_OpenGL
	{
	public:
		BasicMeshRenderData_OpenGL();
		BasicMeshRenderData_OpenGL(const BasicMeshRenderData_OpenGL&) = delete;
		BasicMeshRenderData_OpenGL(BasicMeshRenderData_OpenGL&& other) noexcept = default;
		
		void Clear();

		void Initialize(const IndexedTriangleMesh& mesh);

		inline const auto& GetVertexArray() const { return va; }		
		inline uintMem GetIndexCount() const { return indexCount; }

		BasicMeshRenderData_OpenGL& operator=(const BasicMeshRenderData_OpenGL&) = delete;
		BasicMeshRenderData_OpenGL& operator=(BasicMeshRenderData_OpenGL&& other) noexcept = default;
	private:
		uintMem indexCount;
		OpenGLWrapper::ImmutableStaticGraphicsBuffer buffer;		
		OpenGLWrapper::VertexArray va;
	};	

	class BasicMeshRenderer_OpenGL
	{
	public:
		BasicMeshRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext);

		void SetProjectionMatrix(const Mat4f& matrix);
		void SetViewMatrix(const Mat4f& matrix);
		void SetShadingOptions(Vec3f lightDirection, ColorRGBf ambientColor = 0x000000);
		
		void Render(BasicMeshRenderData_OpenGL& renderData, const Mat4f& modelMatrix, ColorRGBAf color);
	private:
		GraphicsContext_OpenGL& graphicsContext;
		OpenGLWrapper::ShaderProgram program;		
	};
}