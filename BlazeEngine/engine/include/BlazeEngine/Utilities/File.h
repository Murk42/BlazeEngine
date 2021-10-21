#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/String.h"
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
		File(StringView path, FileOpenMode mode, FileOpenFlags flags = FileOpenFlags::None, FilePermission perms = FilePermission::ReadWrite);
		~File();

		int Open(StringView path, FileOpenMode mode, FileOpenFlags flags = FileOpenFlags::None, FilePermission perms = FilePermission::ReadWrite);
		int Close();
		size_t Size();
		size_t Read(Buffer& buffer);		
		size_t Write(BufferView buffer);
		bool IsOpen() const;
	};
}