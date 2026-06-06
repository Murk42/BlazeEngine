#pragma once
#include "BlazeEngine/Core/Common/Stream.h"

namespace Blaze
{
	class ReadSubStream : public ReadStream
	{
	public:
		ReadSubStream(ReadStream& parentStream, uintMem position, uintMem size);

		bool MovePosition(intMem offset) override;
		bool SetPosition(uintMem position) override;
		bool SetPositionFromEnd(intMem position) override;
		uintMem GetPosition() const override;
		uintMem GetSize() const override;

		uintMem Read(void* destinationPtr, uintMem byteCount) override;
	public:
		ReadStream* parentStream;
		uintMem parentOffset;
		uintMem size;
	};
}