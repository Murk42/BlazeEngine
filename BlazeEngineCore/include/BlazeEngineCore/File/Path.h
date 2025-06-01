#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"
#include "BlazeEngineCore/DataStructures/StringUTF8.h"
#include "BlazeEngineCore/DataStructures/StringViewUTF8.h"

namespace Blaze
{
	class String;
	class StringView;
	class StringViewUTF8;

	class BLAZE_CORE_API Path
	{		
	public:
		Path() = default;
		Path(const Path& path) = default;
		Path(Path&& path) noexcept = default;				
		inline Path(String&& string) : value(std::move(string)) {}
		inline Path(StringView&& string) : value(std::move(string)) {}
		inline Path(StringUTF8&& string) : value(std::move(string)) {}
		inline Path(StringViewUTF8&& string) : value(std::move(string)) {}
		template<uintMem C>
		Path(const char(&arr)[C]);

		inline bool Empty() const { return value.Empty(); }

		inline StringViewUTF8 GetString() const { return value; }
		
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
		bool Exists() const;

		uint32 Hash() const;
		
		Path operator/(const Path& other) const;		
		Path& operator/=(const Path& other);		

		bool operator==(const Path& other) const;
		bool operator!=(const Path& other) const;

		Path& operator=(const Path& other) = default;
		Path& operator=(Path&& other) noexcept = default;
		Path& operator=(const String& string);
		Path& operator=(const StringView& string);
		Path& operator=(const StringUTF8& string);
		Path& operator=(const StringViewUTF8& string);
	private:
		StringUTF8 value;		
	};
	template<uintMem C>
	inline Path::Path(const char(&arr)[C])
		: value(arr)
	{
	}
}