#pragma once
#include "BlazeEngine/File/Path.h"
#include "BlazeEngine/File/Stream/FileStream.h"

namespace Blaze
{
	enum class FileAccessPermission
	{
		Read,
		Write,
		ReadWrite
	};			
	enum class FileOpenOptions
	{
		CreateAlways,		//Truncates if the file exists or creates one if it doesnt
		CreateNew,			//Truncates new file, fails if file exists
		OpenAlways,			//Appends if the file exists or creates one if it doesnt
		OpenExisting,		//Appends existing file, fails if it doesn't exist
		TruncateExisting,   //Truncates existing file, fails if it doesn't exist
	};

	enum class FileLifetimeOption
	{
		Normal, //Normal file
		ShortLived, //If possible, the file wont be flushed to disk
		Temporary, //The file is deleted when the last file descriptor is closed		
	};
	enum class FileUsageHint
	{
		Normal,
		RandomAccess,
		Sequential,
	};		
 
	struct FileOpenParameters
	{
		FileOpenOptions openOption = FileOpenOptions::OpenExisting;
		bool createSubdirectories = true;

		FileLifetimeOption locationHint = FileLifetimeOption::Normal;
		FileUsageHint usageHint = FileUsageHint::Normal;
	};

	/*					EXISTS !EXIST TRUNCATES
	                      0       0       0     Impossible
	OPEN_EXISTING         1       0       0
	                      0       1       0     Same as CREATE_NEW
	OPEN_ALWAYS           1       1       0
	                      0       0       1     Impossible
	TRUNCATE_EXISTING     1       0       1 
	CREATE_NEW            0       1       1 
	CREATE_ALWAYS         1       1       1
	
	*/
	
#pragma warning( push )
#pragma warning( disable : 4250)
	class BLAZE_API File : public FileStream
	{		
	public:
		File();
		File(const Path& path, FileAccessPermission mode);
		File(const Path& path, FileAccessPermission mode, FileOpenParameters parameters);
		~File();
		
		Result Open(const Path& path, FileAccessPermission mode);
		Result Open(const Path& path, FileAccessPermission mode, FileOpenParameters parameters);
	};
#pragma warning( pop ) 
}