#pragma once

namespace Blaze
{
	class ConsoleOutputStream : public WriteStream
	{
		WriteStream* linkStream;
	public:
		uintMem Write(const void* ptr, uintMem byteCount) override;
		bool MovePosition(intMem offset) override;
		bool SetPosition(uintMem offset) override;
		bool SetPositionFromEnd(intMem offset) override;
		uintMem GetPosition() const override;
		uintMem GetSize() const override;
	};
}