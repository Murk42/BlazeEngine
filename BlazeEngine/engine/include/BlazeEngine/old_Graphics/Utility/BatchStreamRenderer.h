#pragma once
#include "BlazeEngine/Graphics/GraphicsLibrary.h"

namespace Blaze	
{	
	class BatchStreamRenderer
	{
	public:
		BatchStreamRenderer();

		Result Allocate(uint batchCount, uint batchSize);

		template<typename Vertex, ::std::invocable<uint, uint> F>
		Result Render(ArrayView<Vertex> vertices, const F& renderFunction) const;

		inline const auto& GetBuffer() const { return buffer; }		
	private:
		struct BatchData
		{
			GraphicsLibrary::Fence fence;
		};

		GraphicsLibrary::ImmutableMappedGraphicsBuffer buffer;
		void* bufferMap;
		mutable Array<BatchData> batches;
		mutable uint batch;
		uint batchSize;		

		Result WriteData(const void* ptr, uint size) const;
		Result NextBatch() const;
	};

	template<typename Vertex, ::std::invocable<uint, uint> F>
	Result BatchStreamRenderer::Render(ArrayView<Vertex> vertices, const F& renderFunction) const
	{				
		uint batchVertexCount = batchSize / sizeof(Vertex);
		uint offset = 0;		

		while (true)
		{
			uint vertexCount = std::min<uint>(vertices.Count() - offset, batchVertexCount);

			CHECK_RESULT(WriteData(vertices.Ptr() + offset, vertexCount * sizeof(Vertex)));

			renderFunction(batch * batchVertexCount, vertexCount);

			CHECK_RESULT(NextBatch());

			offset += vertexCount;

			if (offset == vertices.Count())
				break;
		}
				
		return Result();
	}
}