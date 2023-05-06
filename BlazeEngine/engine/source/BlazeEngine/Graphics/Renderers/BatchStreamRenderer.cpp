#include "BlazeEngine/Graphics/Renderers/BatchStreamRenderer.h"

namespace Blaze
{
	BatchStreamRenderer::BatchStreamRenderer()
		: batch(0), bufferMap(nullptr), batchSize(0)
	{

	}
	Result BatchStreamRenderer::Allocate(uint batchCount, uint batchSize)
	{
		this->batchSize = batchSize;

		CHECK_RESULT(buffer.Allocate(nullptr, batchCount * batchSize,
			Graphics::Core::ImmutableGraphicsBufferMapAccess::Write,
			Graphics::Core::ImmutableGraphicsBufferMapType::PersistentCoherent
		));
		bufferMap = buffer.MapBufferRange(0, batchCount * batchSize,
			Graphics::Core::ImmutableGraphicsBufferMapOptions::InvalidateRange
		);
		batches.Resize(batchCount);

		return Result();
	}
	Result BatchStreamRenderer::WriteData(const void* ptr, uint size)
	{
		batches[batch].fence.BlockClient(0.1);

		uint offset = batch * batchSize;
		memcpy((uint8*)bufferMap + offset, ptr, size);

		return Result();
	}
	Result BatchStreamRenderer::NextBatch()
	{
		CHECK_RESULT(batches[batch].fence.SetFence());
		batch = (batch + 1) % batches.Count();

		return Result();
	}
}