#include "BlazeEngine/Graphics/Utility/BatchStreamRenderer.h"

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
			GraphicsLibrary::ImmutableGraphicsBufferMapAccess::Write,
			GraphicsLibrary::ImmutableGraphicsBufferMapType::PersistentCoherent
		));
		bufferMap = buffer.MapBufferRange(0, batchCount * batchSize,
			GraphicsLibrary::ImmutableGraphicsBufferMapOptions::InvalidateRange
		);
		batches.Resize(batchCount);

		return Result();
	}
	Result BatchStreamRenderer::WriteData(const void* ptr, uint size) const
	{
		batches[batch].fence.BlockClient(0.1);

		uint offset = batch * batchSize;
		memcpy((uint8*)bufferMap + offset, ptr, size);

		return Result();
	}
	Result BatchStreamRenderer::NextBatch() const
	{
		CHECK_RESULT(batches[batch].fence.SetFence());
		batch = (batch + 1) % batches.Count();

		return Result();
	}
}