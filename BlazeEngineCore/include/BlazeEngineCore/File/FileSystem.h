#pragma once
#include "BlazeEngineCore/File/Path.h"

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
		
		BLAZE_CORE_API void DeleteFile(const Path& path);
		BLAZE_CORE_API void ResizeFile(const Path& path, uintMem newSize);
		BLAZE_CORE_API uintMem FileSize(const Path& path);

		BLAZE_CORE_API void CreateDirectory(const Path& path);
		BLAZE_CORE_API void CreateDirectoryRecursive(const Path& path);
		BLAZE_CORE_API void DeleteDirectory(const Path& path);

		BLAZE_CORE_API void CopyContents(const Path& destination, const Path& source);
		BLAZE_CORE_API void Copy(const Path& destination, const Path& source, CopyFlags flags = CopyFlags::Overwrite | CopyFlags::Recursive);

		BLAZE_CORE_API TimePoint GetLastWriteTime(const Path& path);
		BLAZE_CORE_API void SetLastWriteTime(const Path& path, const TimePoint& timePoint);
	};
}