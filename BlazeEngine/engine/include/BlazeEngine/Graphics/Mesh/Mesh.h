#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"
#include "BlazeEngine/Graphics/Material/Material.h"

namespace Blaze
{
	enum class MeshFileType
	{
		OBJ
	};

	struct BLAZE_API MeshData
	{
		size_t vertexBufferSize;
		void* vertexBuffer;
		size_t indexBufferSize;
		void* indexBuffer;

		MeshData();
		~MeshData();

		void Load(StringView path, MeshFileType fileType);
	};


	class BLAZE_API Mesh
	{
	public:
		Graphics::Core::VertexArray va;	

		virtual void Render() = 0;
	};	
}