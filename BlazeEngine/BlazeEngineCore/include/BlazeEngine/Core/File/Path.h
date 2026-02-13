#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/String/String.h"
#include "BlazeEngine/Core/String/StringView.h"

namespace Blaze
{
	class BLAZE_API Path
	{
	public:
		Path() = default;
		Path(const Path& path) = default;
		Path(Path&& path) noexcept = default;
		inline Path(u8String&& string) : value(std::move(string)) {}
		inline Path(u8StringView& string) : value(string) {}
		template<uintMem C>
		Path(const char(&arr)[C]);

		uint64 Hash() const;
		inline bool Empty() const { return value.Empty(); }

		inline const u8String& GetString() const { return value; }

		// Root name of "C:/dir1/dir2" is "C:"
		u8String RootName() const;
		// The root directory of "C:/dir1/dir2" is "/"
		u8String RootDirectory() const;
		//	The root path of "C:/dir1/dir2" is "C:/". Effectively same as RootName() + RootDirectory()
		u8String RootPath() const;

		// File name of "C:/dir1/dir2/file.txt" is "file.txt"
		u8String FileName() const;
		// Stem of "C:/dir1/dir2/file.txt" is "file"
		u8String Stem() const;
		// File extension of "C:/dir1/dir2/file.txt" is ".txt"
		u8String FileExtension() const;

		Path ParentPath() const;

		bool IsAbsolute() const;
		bool IsDirectory() const;
		bool IsFile() const;
		bool Exists() const;

		u8StringView Format() const;

		Path operator/(const Path& other) const;
		Path& operator/=(const Path& other);

		bool operator==(const Path& other) const;
		bool operator!=(const Path& other) const;

		Path& operator=(const Path& other) = default;
		Path& operator=(Path&& other) noexcept = default;
		Path& operator=(const u8String& string);
		Path& operator=(const u8StringView& string);
	private:
		u8String value;
	};

	template<uintMem C>
	inline Path::Path(const char(&arr)[C])
		: value(arr)
	{
	}

	template<>
	class Hash<Path>
	{
	public:
		using Type = Path;

		uint64 Compute(const Path& key) const;
	};
}