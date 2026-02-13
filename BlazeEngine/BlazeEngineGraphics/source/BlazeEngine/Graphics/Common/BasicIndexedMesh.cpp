#include "pch.h"
#include "BlazeEngine/Graphics/Common/BasicIndexedMesh.h"
#include "BlazeEngine/Core/File/File.h"
#include "BlazeEngine/Core/Common/Buffer.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace Blaze::Graphics
{
	BasicIndexedMesh::BasicIndexedMesh()
	{
	}
	void BasicIndexedMesh::Clear()
	{
		vertices.Clear();
		indices.Clear();
	}
	bool BasicIndexedMesh::Load(const Path& path, StringView objectName)
	{
		File file{ path, FileAccessPermission::Read };
		return Load(file, path.FileExtension());
	}
	bool BasicIndexedMesh::Load(ReadStream& stream, u8StringView fileExtensionHint, StringView objectName)
	{
		Assimp::Importer importer;
		Buffer streamBuffer{ stream.GetSize() };
		stream.Read(streamBuffer.Ptr(), stream.GetSize());

		auto scene = importer.ReadFileFromMemory(streamBuffer.Ptr(), stream.GetSize(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices, reinterpret_cast<const char*>(u8String(fileExtensionHint).Ptr()));

		if (scene == nullptr)
		{
			BLAZE_LOG_ERROR("Failed to loading a basic triangle mesh. Assimp returned error string: {}", StringView(importer.GetErrorString(), strlen(importer.GetErrorString())));
			return false;
		}

		if (scene->mRootNode->mNumChildren == 0)
		{
			BLAZE_LOG_ERROR("Loading a basic triangle mesh from a scene with no objects");
			return false;
		}

		aiNode* node = nullptr;
		if (objectName.Empty())
			if (scene->mRootNode->mNumChildren == 1)
				node = scene->mRootNode->mChildren[0];
			else
			{
				BLAZE_LOG_ERROR("Loading a basic triangle mesh with no object name from a scene with more than one object");
				return false;
			}
		else
		{
			node = scene->mRootNode->FindNode(objectName.Ptr());
			if (node == nullptr)
			{
				BLAZE_LOG_ERROR("Failed to find a object named \"{}\" while loading a basic triangle mesh", objectName);
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

		indices.ReserveExactly(indexCount);
		vertices.ReserveExactly(vertexCount);

		for (uintMem i = 0; i < node->mNumMeshes; ++i)
		{
			auto mesh = scene->mMeshes[node->mMeshes[i]];

			for (uintMem j = 0; j < mesh->mNumFaces; ++j)
			{
				indices.AddBack(mesh->mFaces[j].mIndices[0]);
				indices.AddBack(mesh->mFaces[j].mIndices[1]);
				indices.AddBack(mesh->mFaces[j].mIndices[2]);
			}

			for (uintMem j = 0; j < mesh->mNumVertices; ++j)
				vertices.AddBack(Vec3f(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z));
		}

		return true;
	}
	void BasicIndexedMesh::Load(Array<Vec3f>&& vertices, Array<uint32>&& indices)
	{
		this->vertices = std::move(vertices);
		this->indices = std::move(indices);
	}
	Array<BasicIndexedMesh::Triangle> BasicIndexedMesh::CreateTriangleArray() const
	{
		Array<Triangle> triangles;
		triangles.ReserveExactly(indices.Count() / 3);

		for (uintMem i = 0; i < triangles.ReservedCount(); ++i)
		{
			Triangle t;
			t.p1 = vertices[indices[i * 3 + 0]];
			t.p2 = vertices[indices[i * 3 + 1]];
			t.p3 = vertices[indices[i * 3 + 2]];

			triangles.AddBack(t);
		}

		return triangles;
	}
}