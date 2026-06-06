#include "pch.h"
#include "BlazeEngine/Core/Common/SubStream.h"

namespace Blaze
{
	ReadSubStream::ReadSubStream(ReadStream& stream, uintMem position, uintMem size)
		: parentStream(&stream), parentOffset(position), size(size)
	{
	}
	bool ReadSubStream::MovePosition(intMem offset)
	{
		return parentStream->MovePosition(offset);
	}
	bool ReadSubStream::SetPosition(uintMem offset)
	{
		return parentStream->SetPosition(offset + parentOffset);
	}
	bool ReadSubStream::SetPositionFromEnd(intMem offset)
	{
		return parentStream->SetPosition(parentOffset + this->size + offset);
	}
	uintMem ReadSubStream::GetPosition() const
	{
		return parentStream->GetPosition() - parentOffset;
	}
	uintMem ReadSubStream::GetSize() const
	{
		return size;
	}
	uintMem ReadSubStream::Read(void* destinationPtr, uintMem byteCount)
	{
		uintMem position = parentStream->GetPosition();

		if (position > parentOffset + size || position < parentOffset)
			return 0;

		if (parentOffset + byteCount > position + size)
		{
			byteCount = position + size - parentOffset;
		}

		return parentStream->Read(destinationPtr, byteCount);
	}
}