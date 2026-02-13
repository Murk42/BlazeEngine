#pragma once
#include "BlazeEngine/Core/Common/Stream.h"

namespace Blaze
{
	class ReadSubStream : public ReadStream
	{
		ReadStream* parentStream;
		uintMem offset;
		uintMem size;
	public:
		ReadSubStream(ReadStream& parentStream, uintMem offset, uintMem size);

		bool MovePosition(intMem offset) override;
		bool SetPosition(uintMem offset) override;
		bool SetPositionFromEnd(intMem offset) override;
		uintMem GetPosition() const override;
		uintMem GetSize() const override;

		uintMem Read(void* ptr, uintMem byteCount) override;
	};
}