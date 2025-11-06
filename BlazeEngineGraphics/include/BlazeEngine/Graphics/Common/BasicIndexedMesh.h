#pragma once
#include "BlazeEngine/Core/Container/Array.h"
#include "BlazeEngine/Core/Container/ArrayView.h"
#include "BlazeEngine/Core/Common/Stream.h"
#include "BlazeEngine/Core/File/Path.h"
#include "BlazeEngine/Core/Math/Vector.h"

namespace Blaze::Graphics
{
	class BasicIndexedMesh
	{
	public:
		struct Triangle
		{
			Vec3f p1;
			Vec3f p2;
			Vec3f p3;
		};

		BasicIndexedMesh();
		BasicIndexedMesh(const BasicIndexedMesh& other) = default;
		BasicIndexedMesh(BasicIndexedMesh&& other) noexcept = default;
		inline BasicIndexedMesh(const Path& path, StringView objectName = StringView()) { Load(path, objectName); }
		inline BasicIndexedMesh(Array<Vec3f>&& vertices, Array<uint32>&& indices) { Load(std::move(vertices), std::move(indices)); }

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
		bool Load(ReadStream& stream, u8StringView fileExtensionHint, StringView objectName = StringView());

		void Load(Array<Vec3f>&& vertices, Array<uint32>&& indices);

		inline ArrayView<uint32> GetIndices() const { return indices; }
		inline ArrayView<Vec3f> GetVertices() const { return vertices; }
		Array<Triangle> CreateTriangleArray() const;

		BasicIndexedMesh& operator=(const BasicIndexedMesh& other) = default;
		BasicIndexedMesh& operator=(BasicIndexedMesh&& other) noexcept = default;
	private:
		Array<uint32> indices;
		Array<Vec3f> vertices;
	};
}