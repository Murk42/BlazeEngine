#pragma once
#include <vector>
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Types.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/Graphics/Buffer.h"
#include "BlazeEngine/Graphics/VertexLayout.h"
#include "BlazeEngine/GameSystem/Resource.h"

namespace Blaze
{	
	class ByteStream;

	class BLAZE_API OldMesh : public Resource<OldMesh>
	{
		String path;

		float* vertex;				
		void* index;				
		uint* materialIndexCounts;
		uint materialCount;

		VertexLayout vl;
		VertexBuffer vb;
		IndexBuffer ib;

		bool LoadBMO(ByteStream&);
		bool LoadOBJ(ByteStream&);
		bool SaveBMO(ByteStream&);
	public:
		OldMesh();
		OldMesh(const OldMesh&);
		OldMesh(OldMesh&&);
		OldMesh(const String& path);
		~OldMesh();

		void SetVertexData(float* vertex, bool copy, unsigned count, const std::vector<Type> layout);
		void SetIndexData(void* index, bool copy, unsigned count, Type indexType);				
		void SetMaterialData(unsigned* materialIndexCounts, bool copy, unsigned materialCount);

		inline float* GetVertex() const { return vertex; }		
		inline VertexLayout& GetVertexLayout() { return vl; }
		inline const VertexLayout& GetVertexLayout() const { return vl; }
		inline VertexBuffer& GetVertexBuffer() { return vb; }
		inline const Buffer& GetVertexBuffer() const { return vb; }

		inline void* GetIndex() const;		
		inline IndexBuffer& GetIndexBuffer() { return ib; }
		inline const IndexBuffer& GetIndexBuffer() const { return ib; }

		inline unsigned* GetIndexMaterialCounts() const { return materialIndexCounts; }
		inline unsigned GetMaterialCount() const { return materialCount; }			

		bool Load(const String& path);
		bool Save(const String& path);		

		void operator=(const OldMesh&);
		void operator=(OldMesh&&);
	};
}