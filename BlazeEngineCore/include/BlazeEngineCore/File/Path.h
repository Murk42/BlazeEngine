#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"

namespace Blaze
{
	class BLAZE_CORE_API Path
	{		
	public:
		Path(); 
		Path(const Path& path);
		Path(Path&& path) noexcept;		
		Path(const StringViewUTF8& string);
		template<uintMem C>
		Path(const char(&arr)[C]);

		StringUTF8 ToString() const;
		
		// Root name of "C:/dir1/dir2" is "C:"				
		StringUTF8 RootName() const;		
		// The root directory of "C:/dir1/dir2" is "/"		
		StringUTF8 RootDirectory() const;		
		//	The root path of "C:/dir1/dir2" is "C:/". Effectively same as RootName() + RootDirectory()		
		StringUTF8 RootPath() const;
		
		// File name of "C:/dir1/dir2/file.txt" is "file.txt"				
		StringUTF8 FileName() const;		
		// Stem of "C:/dir1/dir2/file.txt" is "file"				
		StringUTF8 Stem() const;		
		// File extension of "C:/dir1/dir2/file.txt" is ".txt"					
		StringUTF8 FileExtension() const;

		Path ParentPath() const;

		bool IsAbsolute() const;
		bool IsDirectory() const;
		bool IsFile() const;
		bool Empty() const;		
		bool Exists() const;

		uint32 Hash() const;
		
		Path operator/(const Path& other) const;
		Path operator/(const StringView& other) const;
		Path operator/(const StringViewUTF8& other) const;

		bool operator==(const Path& other) const;
		bool operator!=(const Path& other) const;

		Path& operator=(const Path& other);
		Path& operator=(Path&& other) noexcept;
		Path& operator=(const StringView& string);
		Path& operator=(const StringViewUTF8& string);		
		
		Path(const std::filesystem::path& path);
		std::filesystem::path& GetUnderlyingObject();
		const std::filesystem::path& GetUnderlyingObject() const;
	private:
		std::filesystem::path path;
	};
	template<uintMem C>
	inline Path::Path(const char(&arr)[C])
		: Path(StringView(arr))
	{
	}
}