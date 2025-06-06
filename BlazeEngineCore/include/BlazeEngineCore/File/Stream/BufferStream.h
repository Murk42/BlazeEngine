#pragma once
#include "BlazeEngineCore/File/Stream/Stream.h"
#include "BlazeEngineCore/DataStructures/Array.h"

namespace Blaze
{
#pragma warning( push )
#pragma warning( disable : 4250)	
	class BLAZE_CORE_API BufferStreamBase : virtual public StreamBase
	{
	public:
		BufferStreamBase();
		BufferStreamBase(const BufferStreamBase&) = delete;
		BufferStreamBase(BufferStreamBase&&) noexcept;
		~BufferStreamBase();

		bool Empty() const;
		void Clear();

		bool MovePosition(intMem offset) override;
		bool SetPosition(uintMem offset) override;
		bool SetPositionFromEnd(intMem offset) override;

		uintMem GetPosition() const override;

		uintMem GetSize() const override;
		void* GetPtr();
		const void* GetPtr() const;

		BufferStreamBase& operator=(const BufferStreamBase&) = delete;
		BufferStreamBase& operator=(BufferStreamBase&&) noexcept;
	protected:
		Array<byte> buffer;
		uintMem position;
	};

	class BLAZE_CORE_API BufferWriteStream : virtual public BufferStreamBase, virtual public WriteStream
	{
	public:
		BufferWriteStream();
		BufferWriteStream(BufferWriteStream&& other) = default;
		~BufferWriteStream();

		uintMem Write(const void* ptr, uintMem byteCount) override;

		BufferWriteStream& operator=(BufferWriteStream&&) noexcept = default;
	};

	class BLAZE_CORE_API BufferReadStream : virtual public BufferStreamBase, virtual public ReadStream
	{
	public:
		BufferReadStream();				
		BufferReadStream(BufferReadStream&& other) = default;
		~BufferReadStream();

		uintMem Read(void* ptr, uintMem byteCount) override;

		BufferReadStream& operator=(BufferReadStream&&) noexcept = default;
	};

	class BLAZE_CORE_API BufferStream : public BufferReadStream, public BufferWriteStream
	{
	public:
		BufferStream() = default;
		BufferStream(BufferStream&& other) = default;

		BufferStream& operator=(BufferStream&&) noexcept = default;
	};	

	class BLAZE_CORE_API BufferViewStream : public ReadStream
	{
	public:
		BufferViewStream();
		BufferViewStream(const BufferViewStream&) = delete;
		BufferViewStream(BufferViewStream&&) noexcept;
		BufferViewStream(const void* ptr, uintMem size);
		~BufferViewStream();

		inline bool Empty() const { return size == 0; }
		void Clear();

		bool MovePosition(intMem offset) override;
		bool SetPosition(uintMem offset) override;
		bool SetPositionFromEnd(intMem offset) override;

		uintMem GetPosition() const override;

		uintMem GetSize() const override;
		const void* GetPtr() const;

		uintMem Read(void* ptr, uintMem byteCount) override;

		/*
			Replaces the internal buffer. Sets the position to the beginning
		*/
		void SetBuffer(const void* ptr, uintMem size);		

		BufferViewStream& operator=(const BufferViewStream&) = delete;
		BufferViewStream& operator=(BufferViewStream&&) noexcept;
	protected:
		const void* ptr;
		uintMem size;
		uintMem position;
	};
#pragma warning( pop ) 
}