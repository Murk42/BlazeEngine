#pragma once
#include "BlazeEngine/File/Stream/Stream.h"

namespace Blaze
{
	class ReadSubStream : public ReadStream
	{
		ReadStream* parentStream;
		uint offset;
		uint size;
	public:
		ReadSubStream(ReadStream& parentStream, uint offset, uint size);

		bool MovePosition(int64 offset) override;
		bool SetPosition(uint64 offset) override;
		bool SetPositionFromEnd(int64 offset) override;
		uint64 GetPosition() const override;
		uint64 GetSize() const override;

		uint64 Read(void* ptr, uint64 byteCount) override;
	};
}