#pragma once
#include "BlazeEngine/Core/File/Path.h"
#include "BlazeEngine/Core/Time/TimePoint.h"

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

		BLAZE_API Path GetCurrentPath();

		BLAZE_API void DeleteFile(const Path& path);
		BLAZE_API void ResizeFile(const Path& path, uintMem newSize);
		BLAZE_API uintMem FileSize(const Path& path);

		BLAZE_API void CreateDirectory(const Path& path);
		BLAZE_API void CreateDirectoryRecursive(const Path& path);
		BLAZE_API void DeleteDirectory(const Path& path);

		BLAZE_API void CopyContents(const Path& destination, const Path& source);
		BLAZE_API void Copy(const Path& destination, const Path& source, CopyFlags flags = CopyFlags::Overwrite | CopyFlags::Recursive);

		//TODO NOT IMPLEMENTED
		BLAZE_API ProgramTimePoint GetLastWriteTime(const Path& path);
		BLAZE_API void SetLastWriteTime(const Path& path, const ProgramTimePoint& timePoint);
	};
}