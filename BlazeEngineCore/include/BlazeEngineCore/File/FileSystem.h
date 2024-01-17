#pragma once
#include "BlazeEngineCore/File/Path.h"
#include "BlazeEngineCore/Debug/ResultValue.h"

namespace Blaze
{
	namespace FileSystem
	{
		enum class CopyFlags
		{
			Update = 2, //If the file destination file is newer the function returns Result::DidntUpdateFile
			Overwrite = 4, //Overwrites the file

			Recursive = 16, //Copies the subfolders and their contents
			FoldersOnly = 4096 //Copies only the folders
		};

		ENUM_CLASS_BITWISE_OPERATIONS(CopyFlags)		

		BLAZE_CORE_API Path GetCurrentPath();
		
		BLAZE_CORE_API Result DeleteFile(const Path& path);
		BLAZE_CORE_API Result ResizeFile(const Path& path, uintMem newSize);
		BLAZE_CORE_API uintMem FileSize(const Path& path);

		BLAZE_CORE_API Result CreateDirectory(const Path& path);
		BLAZE_CORE_API Result CreateDirectoryRecursive(const Path& path);
		BLAZE_CORE_API Result DeleteDirectory(const Path& path);		

		BLAZE_CORE_API Result CopyContents(const Path& destination, const Path& source);
		BLAZE_CORE_API Result Copy(const Path& destination, const Path& source, CopyFlags flags = CopyFlags::Overwrite | CopyFlags::Recursive);
	};
}