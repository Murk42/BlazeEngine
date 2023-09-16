#pragma once
#include "BlazeEngine/File/Stream/Stream.h"

namespace Blaze
{
	class BLAZE_API BufferStreamBase : virtual public StreamBase
	{
	public:
		BufferStreamBase();
		BufferStreamBase(uint64 size);
		BufferStreamBase(void* buffer, uint64 size);
		BufferStreamBase(const FileStreamBase&) = delete;
		~BufferStreamBase();

		Result SetBuffer(void* buffer, uint64 size);
		Result Clear();

		bool MovePosition(int64 offset) override;
		bool SetPosition(uint64 offset) override;
		uint64 GetPosition() const override;

		uint64 GetSize() const override;
		const void* GetBuffer() const;

		FileStreamBase& operator=(const FileStreamBase&) = delete;
	protected:
		void* buffer;
		uint64 size;
		uint64 position;
	};

	class BLAZE_API BufferWriteStream : virtual public BufferStreamBase, virtual public WriteStream
	{
	public:
		BufferWriteStream();
		BufferWriteStream(uint64 size);
		BufferWriteStream(void* buffer, uint64 size);
		~BufferWriteStream();

		uint64 Write(const void* ptr, uint64 byteCount) override;

		using BufferStreamBase::GetBuffer;
	};

	class BLAZE_API BufferReadStream : virtual public BufferStreamBase, virtual public ReadStream
	{
	public:
		BufferReadStream();
		BufferReadStream(uint64 size);
		BufferReadStream(void* buffer, uint64 size);
		~BufferReadStream();

		uint64 Read(void* ptr, uint64 byteCount) override;
	};

	class BufferStream : public BufferReadStream, public BufferWriteStream
	{

	};
}