#include "BlazeEngine/File/Stream/SubStream.h"

namespace Blaze
{
	ReadSubStream::ReadSubStream(ReadStream& stream, uint offset, uint size)
		: parentStream(&stream), offset(offset), size(size)
	{
	}
	bool ReadSubStream::MovePosition(int64 offset)
	{
		return parentStream->MovePosition(offset);
	}
	bool ReadSubStream::SetPosition(uint64 offset)
	{
		return parentStream->SetPosition(offset + this->offset);
	}
	bool ReadSubStream::SetPositionFromEnd(int64 offset)
	{
		return parentStream->SetPosition(this->offset + this->size + offset);
	}
	uint64 ReadSubStream::GetPosition() const
	{
		return parentStream->GetPosition() - offset;
	}
	uint64 ReadSubStream::GetSize() const
	{
		return size;
	}
	uint64 ReadSubStream::Read(void* ptr, uint64 byteCount)
	{
		uint64 position = parentStream->GetPosition();

		if (position > offset + size || position < offset)
			return 0;

		if (position + byteCount > offset + size)
		{

			byteCount = offset + size - position;
		}

		return parentStream->Read(ptr, byteCount);
	}
}