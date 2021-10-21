#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Tuple.h"
#include "BlazeEngine/Graphics/Buffer.h"
#include "BlazeEngine/Graphics/VertexLayout.h"

namespace Blaze
{
	template<typename ... T>
	struct Vertex
	{
		using TemplateGroup = TemplateGroup<T...>;		
		constexpr static Type types[sizeof ... (T)]{ GetType<T>()... };

		Tuple<T...> values;

		constexpr Vertex() { }
		constexpr Vertex(const T& ... values) : values(values...) { }

		template<size_t index>
		constexpr auto& GetValue()
		{
			return values.GetValueByIndex<index>();
		}
		template<size_t index>
		constexpr const auto& GetValue() const
		{
			return values.GetValueByIndex<index>();
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
		void SetVertices(const Vertex<T...>* ptr, size_t count)
		{
			SetVertices(ptr, count, { GetType<T>()... });
		}
		void SetVertices(const void* ptr, uint count, const std::vector<Type>& layout);		
		void ChangeVertices(const void* ptr, uint count, size_t offset);

		template<typename T>
		void SetIndicies(const T* ptr, uint count)
		{
			ib.AllocateData(ptr, count * SizeOf(GetType<T>()), BufferUsage::Static | BufferUsage::Draw, GetType<T>());
		}
		void SetIndicies(const void* ptr, uint count, Type indexType);
		void ChangeIndicies(const void* ptr, uint count, size_t offset);
	};
}