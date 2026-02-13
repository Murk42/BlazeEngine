#pragma once
#include "BlazeEngine/Core/Common/Stream.h"

namespace Blaze
{
#pragma warning( push )
#pragma warning( disable : 4250)
	class BLAZE_API FileStreamBase : virtual public StreamBase
	{
	public:
		FileStreamBase();
		FileStreamBase(FileStreamBase&& other) noexcept;
		~FileStreamBase() override;

		void AcquireHandle(void* file);
		bool IsOpen() const;

		void Close();
		void Flush();

		bool MovePosition(intMem offset) override;
		bool SetPosition(uintMem offset) override;
		bool SetPositionFromEnd(intMem offset) override;
		uintMem GetPosition() const override;

		uintMem GetSize() const override;

		inline void* GetHandle() const { return file; }

		inline uint32 Hash() const { return static_cast<uint32>(reinterpret_cast<uintMem>(file)); }

		FileStreamBase& operator=(FileStreamBase&& other) noexcept;
	private:
		void* file;
	};

	class BLAZE_API FileWriteStream : virtual public FileStreamBase, virtual public WriteStream
	{
	public:
		uintMem Write(const void* ptr, uintMem byteCount) override;
	protected:
		FileWriteStream();
		FileWriteStream(FileWriteStream&&) noexcept;
		~FileWriteStream() override;

		FileWriteStream& operator=(FileWriteStream&&) noexcept;
	};

	class BLAZE_API FileReadStream : virtual public FileStreamBase, virtual public ReadStream
	{
	public:
		uintMem Read(void* ptr, uintMem byteCount) override;
	protected:
		FileReadStream();
		FileReadStream(FileReadStream&&) noexcept;
		~FileReadStream() override;

		FileReadStream& operator=(FileReadStream&&) noexcept;
	};

	class BLAZE_API FileStream : public FileReadStream, public FileWriteStream
	{
	public:
		FileStream();
		FileStream(FileStream&& other) noexcept;

		FileStream& operator=(FileStream&& other) noexcept;
	};
#pragma warning( pop )
}