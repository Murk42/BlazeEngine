#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/String.h"

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


		int BLAZE_API CreateFile(StringView path);
		int BLAZE_API CreateFolder(StringView path);
		int BLAZE_API DeleteFile(StringView path);
		int BLAZE_API DeleteFolder(StringView path);

		int BLAZE_API Exists(StringView path);
		int BLAZE_API Copy(StringView destination, StringView source, CopyFlags flags = CopyFlags::Overwrite | CopyFlags::Recursive);
	};
}