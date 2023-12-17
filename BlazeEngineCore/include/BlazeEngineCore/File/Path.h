#pragma once
#include <utility>

namespace Blaze
{
	class BLAZE_CORE_API Path
	{
		String value;
	public:
		Path() : value() { }
		Path(const Path& path) : value(path.value) { }
		Path(Path&& path) noexcept : value(std::move(path.value)) { }
		Path(const String& string) : value(string) { }
		Path(String&& string) noexcept : value(std::move(string)) { }
		Path(const StringView& string) : value(string) { }
		Path(const char* ptr) : value(ptr) { }

		StringView GetString() const { return value; }

		/// <summary>				
		/// Root name of "C:/dir1/dir2" is "C:"		
		/// </summary>		
		String RootName() const;
		/// <summary>
		/// The root directory of "C:/dir1/dir2" is "/"
		/// </summary>		
		String RootDirectory() const;
		/// <summary>
		///	The root path of "C:/dir1/dir2" is "C:/". Effectively same as RootName() + RootDirectory()
		/// </summary>		
		String RootPath() const;

		/// <summary>
		/// File name of "C:/dir1/dir2/file.txt" is "file.txt"		
		/// </summary>		
		String FileName() const;
		/// <summary>
		/// Stem of "C:/dir1/dir2/file.txt" is "file"		
		/// </summary>		
		String Stem() const;
		/// <summary>
		/// File extension of "C:/dir1/dir2/file.txt" is ".txt"		
		/// </summary>		
		String FileExtension() const;		

		bool IsAbsolute() const;
		bool IsDirectory() const;
		bool IsFile() const;
		bool IsEmpty() const;

		Path operator/(StringView other) const
		{
			Path out;
			out.value = value + '/' + other;
			return out;
		}

		Path& operator=(const Path& other) noexcept
		{
			value = other.value;
			return *this;
		}
		Path& operator=(Path&& other) noexcept
		{
			value = std::move(other.value);
			return *this;
		}
		Path& operator=(const StringView& string) noexcept
		{
			value = string;
			return *this;
		}
		Path& operator=(const String& string) noexcept
		{
			value = string;
			return *this;
		}
		Path& operator=(String&& string) noexcept
		{
			value = std::move(string);
			return *this;
		}
		Path& operator=(const char* ptr) noexcept
		{
			value = ptr;
			return *this;
		}
		
		bool operator==(const Path& other) const
		{
			return value == other.value;
		}
		bool operator!=(const Path& other) const
		{
			return value != other.value;
		}
	};
}