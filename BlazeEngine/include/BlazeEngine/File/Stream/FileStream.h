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

		bool MovePosition(intMem offset) override;
		bool SetPosition(uintMem offset) override;
		bool SetPositionFromEnd(intMem offset) override;
		uintMem GetPosition() const override;

		uintMem GetSize() const override;

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

		uintMem Write(const void* ptr, uintMem byteCount) override;
	};

	class BLAZE_API FileReadStream : virtual public FileStreamBase, virtual public ReadStream
	{
	public:
		FileReadStream();
		FileReadStream(void* file);
		~FileReadStream();

		uintMem Read(void* ptr, uintMem byteCount) override;
	};

	class BLAZE_API FileStream : public FileReadStream, public FileWriteStream
	{

	};
#pragma warning( pop ) 
}