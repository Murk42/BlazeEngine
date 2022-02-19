#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/File/Path.h"

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

		BLAZE_API int CreateFile(const Path& path);
		BLAZE_API int DeleteFile(const Path& path);
		BLAZE_API int ResizeFile(const Path& path, size_t newSize);
		BLAZE_API int FileSize(const Path& path, size_t& size);

		BLAZE_API int CreateDirectory(const Path& path);
		BLAZE_API int DeleteDirectory(const Path& path);

		BLAZE_API int Exists(const Path& path);

		BLAZE_API int CopyContents(const Path& destination, const Path& source);
		BLAZE_API int Copy(const Path& destination, const Path& source, CopyFlags flags = CopyFlags::Overwrite | CopyFlags::Recursive);
	};
}