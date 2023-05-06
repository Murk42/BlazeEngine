#pragma once
#include "BlazeEngine/Graphics/GraphicsLibrary.h"
#include <concepts>

namespace Blaze	
{	
	class BatchStreamRenderer
	{
	public:
		BatchStreamRenderer();

		Result Allocate(uint batchCount, uint batchSize);

		template<typename Vertex, std::invocable<uint, uint> F>
		Result Render(Vertex* vertices, uint count, const F& renderFunction);

		inline const auto& GetBuffer() const { return buffer; }		
	private:
		struct BatchData
		{
			Graphics::Core::Fence fence;
		};

		Graphics::Core::ImmutableMappedGraphicsBuffer buffer;
		void* bufferMap;
		Array<BatchData> batches;
		uint batch;
		uint batchSize;

		Result WriteData(const void* ptr, uint size);
		Result NextBatch();
	};

	template<typename Vertex, std::invocable<uint, uint> F>
	Result BatchStreamRenderer::Render(Vertex* vertices, uint count, const F& renderFunction)
	{
		uint batchVertexCount = batchSize / sizeof(Vertex);
		uint offset = 0;

		while (true)
		{
			uint vertexCount = std::min<uint>(count - offset, batchVertexCount);

			CHECK_RESULT(WriteData(vertices + offset, vertexCount * sizeof(Vertex)));

			renderFunction(batch * batchVertexCount, vertexCount);

			CHECK_RESULT(NextBatch());

			offset += vertexCount;

			if (offset == count)
				break;
		}

		return Result();
	}
}