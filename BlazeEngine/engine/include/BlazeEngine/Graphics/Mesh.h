#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Tuple.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"

namespace Blaze
{
	enum class IndexType
	{
		UnsignedByte = 5121,
		UnsignedShort = 5123,
		UnsignedInt = 5125,
	};

	template<IndexType T> struct GetTypeStruct { using type = InvalidType; };
	template<> struct GetTypeStruct<IndexType::UnsignedByte> { using type = byte; };
	template<> struct GetTypeStruct<IndexType::UnsignedShort> { using type = short; };
	template<> struct GetTypeStruct<IndexType::UnsignedInt> { using type = unsigned; };
	template<IndexType T> using GetType = GetTypeStruct<T>::template type;

	template<typename T> struct GetIndexTypeStruct { static constexpr IndexType type = (IndexType)0; };
	template<> struct GetIndexTypeStruct<byte> { static constexpr IndexType type = IndexType::UnsignedByte; };
	template<> struct GetIndexTypeStruct<short> { static constexpr IndexType type = IndexType::UnsignedShort; };
	template<> struct GetIndexTypeStruct<unsigned> { static constexpr IndexType type = IndexType::UnsignedInt; };
	template<typename T> static constexpr IndexType GetIndexType = GetIndexTypeStruct<T>::template type;

	inline size_t SizeOf(IndexType type)
	{
		switch (type)
		{
		case Blaze::IndexType::UnsignedByte: return sizeof(GetType<IndexType::UnsignedByte>);
		case Blaze::IndexType::UnsignedShort: return sizeof(GetType<IndexType::UnsignedShort>);
		case Blaze::IndexType::UnsignedInt: return sizeof(GetType<IndexType::UnsignedInt>);
		}
	}

	struct BaseMesh
	{
		Graphics::VertexArray va;		
		
		BaseMesh() = default;
		BaseMesh(const BaseMesh&) = delete;
		BaseMesh(BaseMesh&& bm) noexcept
			: va(std::move(bm.va))
		{
		}

		BaseMesh& operator=(const BaseMesh&) = delete;
		BaseMesh& operator=(BaseMesh&& bm) noexcept
		{
			va = std::move(bm.va);			
			return *this;
		}
	};

	template<typename ... T>
	struct MeshVertex
	{
		using TemplateGroup = TemplateGroup<T...>;

		Tuple<T...> tuple;

		MeshVertex()
			: tuple()
		{
		}
		MeshVertex(const Tuple<T...>& t)
			: tuple(t)
		{
		}
		MeshVertex(Tuple<T...>&& t) noexcept
			: tuple(std::move(t))
		{
		}
		MeshVertex(const T& ... args)
			: tuple(args...)
		{

		}

		MeshVertex& operator=(const Tuple<T...>& v)
		{
			tuple = v;
			return *this;
		}
		MeshVertex& operator=(Tuple<T...>&& t) noexcept
		{
			tuple = std::move(t);
			return *this;
		}

		static constexpr Graphics::AttributeType typeArray[]{ Graphics::GetAttributeType<T>... };
		static constexpr size_t stride = (sizeof(T) + ...);
	};	

	/*
	template<typename _VertexType, typename _IndexType, MeshUsage _meshUsage>
	class StaticMesh : public BaseMesh
	{
		static constexpr MeshUsage meshUsage = _meshUsage;
		uint vertexCount;
		uint indexCount;
	public:
		using Vertex = _VertexType;
		using Index = _IndexType;

		StaticMesh()
			: BaseMesh()
		{
			uint offset = 0;

			va.Bind();
			for (uint i = 0; i < Vertex::TemplateGroup::Size; ++i)
			{
				va.EnableVertexAttribArray(i);
				va.VertexAttribPointer(i, Graphics::Attribute(Vertex::typeArray[i], false), Vertex::stride, offset);
				offset += Graphics::SizeOf(Vertex::typeArray[i]);
			}
			va.Unbind();
		}
		StaticMesh(const StaticMesh&) = delete;
		StaticMesh(StaticMesh&& sm)
			: BaseMesh(std::move(sm))
		{

		}

		void AllocateVertices(uint count, const Vertex* vertices)
		{			
			ab.AllocateData(count * Vertex::stride, vertices, (Graphics::BufferUsage)meshUsage);
		}
		void ChangeVertices(uint count, const Vertex* vertices, uint offset)
		{			
			ab.ChangeData(count * Vertex::stride, vertices, offset * Vertex::stride);
		}
		void AllocateIndices(uint count, const Index* indices)
		{			
			ib.AllocateData(count * sizeof(IndexType), indices, (Graphics::BufferUsage)meshUsage);
		}
		void ChangeIndices(uint count, const Index* indices, uint offset)
		{			
			ib.ChangeData(count * sizeof(Index), indices, offset * sizeof(Index));
		}

		inline uint GetVertexCount() const { return vertexCount; }
		inline uint GetIndexCount() const { return indexCount; }
		constexpr MeshUsage GetMeshUsage() const { return meshUsage; }

		StaticMesh& operator=(const StaticMesh&) = delete;
		StaticMesh& operator=(StaticMesh&& sm) noexcept
		{
			BaseMesh::operator=(std::move(sm));

			return *this;
		}
	};
	*/	
}