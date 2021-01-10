#pragma once
#include <vector>
#include "BlazeEngine/Core/EngineCore.h"
#include "Buffer.h"

namespace Blaze
{
	class BLAZE_API VertexLayout
	{		
		uint id;
		std::vector<Type> layout;
		VertexBuffer* vertexBuffer;
		IndexBuffer* indexBuffer;		
	public:
		VertexLayout();
		VertexLayout(const VertexLayout&);
		VertexLayout(VertexLayout&&) noexcept;
		VertexLayout(const std::vector<Type>& layout, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer);
		~VertexLayout();

		void SetLayout(const std::vector<Type>& layout);
		void BindVertexBuffer(VertexBuffer* vb);
		void BindIndexBuffer(IndexBuffer* ib);
		inline const std::vector<Type>& GetLayout() const { return layout; }
		inline size_t GetStride() const
		{
			size_t sum = 0; 
			for (const auto& t : layout) 
				sum += SizeOf(t); 
			return sum; 
		}
		inline VertexBuffer* GetVertexBuffer() const { return vertexBuffer; }
		inline IndexBuffer* GetIndexBuffer() const { return indexBuffer; }

		void Bind() const;
		static void Unbind();
		static VertexLayout* GetBound();

		void operator=(const VertexLayout&);
		void operator=(VertexLayout&&) noexcept;

		friend class Buffer;
	};		
}