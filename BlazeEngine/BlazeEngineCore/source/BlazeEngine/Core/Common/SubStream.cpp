#include "pch.h"
#include "BlazeEngine/Core/Common/SubStream.h"

namespace Blaze
{
	ReadSubStream::ReadSubStream(ReadStream& stream, uintMem offset, uintMem size)
		: parentStream(&stream), offset(offset), size(size)
	{
	}
	bool ReadSubStream::MovePosition(intMem offset)
	{
		return parentStream->MovePosition(offset);
	}
	bool ReadSubStream::SetPosition(uintMem offset)
	{
		return parentStream->SetPosition(offset + this->offset);
	}
	bool ReadSubStream::SetPositionFromEnd(intMem offset)
	{
		return parentStream->SetPosition(this->offset + this->size + offset);
	}
	uintMem ReadSubStream::GetPosition() const
	{
		return parentStream->GetPosition() - offset;
	}
	uintMem ReadSubStream::GetSize() const
	{
		return size;
	}
	uintMem ReadSubStream::Read(void* ptr, uintMem byteCount)
	{
		uintMem position = parentStream->GetPosition();

		if (position > offset + size || position < offset)
			return 0;

		if (position + byteCount > offset + size)
		{

			byteCount = offset + size - position;
		}

		return parentStream->Read(ptr, byteCount);
	}
}