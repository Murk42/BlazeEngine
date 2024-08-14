#pragma once
#include "BlazeEngineCore/File/Stream/Stream.h"

namespace Blaze
{
#pragma warning( push )
#pragma warning( disable : 4250)	
	class BLAZE_CORE_API BufferStreamBase : virtual public StreamBase
	{
	public:
		BufferStreamBase();
		BufferStreamBase(uintMem size);
		BufferStreamBase(void* buffer, uintMem size);
		BufferStreamBase(const BufferStreamBase&) = delete;
		~BufferStreamBase();

		Result SetBuffer(void* buffer, uintMem size);
		Result Clear();

		bool MovePosition(intMem offset) override;
		bool SetPosition(uintMem offset) override;
		bool SetPositionFromEnd(intMem offset) override;

		uintMem GetPosition() const override;

		uintMem GetSize() const override;
		const void* GetBuffer() const;

		BufferStreamBase& operator=(const BufferStreamBase&) = delete;
	protected:
		void* buffer;
		uintMem size;
		uintMem position;
	};

	class BLAZE_CORE_API BufferWriteStream : virtual public BufferStreamBase, virtual public WriteStream
	{
	public:
		BufferWriteStream();
		BufferWriteStream(uintMem size);
		BufferWriteStream(void* buffer, uintMem size);
		~BufferWriteStream();

		uintMem Write(const void* ptr, uintMem byteCount) override;

		using BufferStreamBase::GetBuffer;
	};

	class BLAZE_CORE_API BufferReadStream : virtual public BufferStreamBase, virtual public ReadStream
	{
	public:
		BufferReadStream();
		BufferReadStream(uintMem size);
		BufferReadStream(void* buffer, uintMem size);
		~BufferReadStream();

		uintMem Read(void* ptr, uintMem byteCount) override;
	};

	class BufferStream : public BufferReadStream, public BufferWriteStream
	{

	};
#pragma warning( pop ) 
}