#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/OrderedTuple.h"
#include "BlazeEngine/Graphics/Buffer.h"
#include "BlazeEngine/Graphics/VertexLayout.h"

namespace Blaze
{
	template<typename ... T>
	struct Vertex
	{
		constexpr static size_t size = sizeof ... (T);
		constexpr static Type types[sizeof ... (T)]{ GetType<T>()... };

		OrderedTuple<T...> values;

		constexpr Vertex() { }
		constexpr Vertex(const T& ... values) : values(values...) { }

		template<uint index>
		constexpr at_index_t<index, T...>& GetValue()
		{
			return values.GetValue<index>();
		}
		template<uint index>
		constexpr const at_index_t<index, T...>& GetValue() const
		{
			return values.GetValue<index>();
		}

		constexpr std::vector<Type> GetLayout() const
		{
			return { GetType<T>()... };
		}
	};

	class BLAZE_API Mesh
	{
	public:
		IndexBuffer ib;
		VertexBuffer vb;
		VertexLayout vl;

		Mesh();

		template<typename ... T>
		void SetVertices(const Vertex<T...>* ptr, uint count)
		{
			SetVertices(ptr, count, { GetType<T>()... });
		}
		void SetVertices(const void* ptr, uint count, const std::vector<Type>& layout);		
		void ChangeVertices(const void* ptr, uint count, uint offset);

		template<typename T>
		void SetIndicies(const T* ptr, uint count)
		{
			ib.AllocateData(ptr, count * SizeOf(GetType<T>()), Static | Draw, GetType<T>());
		}
		void SetIndicies(const void* ptr, uint count, Type indexType);
		void ChangeIndicies(const void* ptr, uint count, uint offset);
	};
}