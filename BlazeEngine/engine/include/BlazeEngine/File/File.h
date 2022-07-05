#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Core/Result.h"
#include "BlazeEngine/File/Path.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/Buffer.h"

namespace Blaze
{
	enum class FileOpenMode
	{
		Read = 0,
		Write = 1,
		ReadWrite = 2
	};
	enum class FileOpenFlags
	{
		None = 0,
		Append = 8,
		Create = 256,
		Truncate = 512,
		Excel = 1024
	};
	enum class FilePermission
	{
		Read = 256,
		Write = 128,
		ReadWrite = 384
	};

	ENUM_CLASS_BITWISE_OPERATIONS(FileOpenFlags)
	
	class BLAZE_API File
	{
		int fd;
	public:
		File();
		File(const Path& path, FileOpenMode mode, FileOpenFlags flags = FileOpenFlags::None, FilePermission perms = FilePermission::ReadWrite);
		~File();

		Result Open(const Path& path, FileOpenMode mode, FileOpenFlags flags = FileOpenFlags::None, FilePermission perms = FilePermission::ReadWrite);
		Result Close();
		size_t Size();
		//Puts the buffer size amount of bytes from the file into the buffer without allocating or freeing it.
		size_t Read(Buffer& buffer);		
		size_t Write(BufferView buffer);
		bool IsOpen() const;
	};
}