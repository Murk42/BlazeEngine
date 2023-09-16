#pragma once
#include "BlazeEngine/File/Stream/Stream.h"

namespace Blaze
{
#pragma warning( push )
#pragma warning( disable : 4250)	
	class BLAZE_API FileStreamBase : virtual public StreamBase
	{
	public:
		FileStreamBase();
		FileStreamBase(void* file);
		FileStreamBase(const FileStreamBase&) = delete;
		~FileStreamBase();

		Result Open(void* file);
		bool IsOpen() const;

		Result Close();
		Result Flush();

		bool MovePosition(int64 offset) override;
		bool SetPosition(uint64 offset) override;
		bool SetPositionFromEnd(int64 offset) override;
		uint64 GetPosition() const override;

		uint64 GetSize() const override;

		inline void* GetHandle() const { return file; }

		FileStreamBase& operator=(const FileStreamBase&) = delete;
	private:
		void* file;
	};

	class BLAZE_API FileWriteStream : virtual public FileStreamBase, virtual public WriteStream
	{
	public:
		FileWriteStream();
		FileWriteStream(void* file);
		~FileWriteStream();

		uint64 Write(const void* ptr, uint64 byteCount) override;
	};

	class BLAZE_API FileReadStream : virtual public FileStreamBase, virtual public ReadStream
	{
	public:
		FileReadStream();
		FileReadStream(void* file);
		~FileReadStream();

		uint64 Read(void* ptr, uint64 byteCount) override;
	};

	class BLAZE_API FileStream : public FileReadStream, public FileWriteStream
	{

	};
#pragma warning( pop ) 
}