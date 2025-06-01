#include "pch.h"
#include "BlazeEngineCore/File/Path.h"
#include "BlazeEngineCore/DataStructures/StringUTF8.h"
#include "BlazeEngineCore/DataStructures/StringView.h"
#include "BlazeEngineCore/DataStructures/StringViewUTF8.h"
#include "BlazeEngineCore/Debug/Logger.h"
#include <filesystem>

#define PATH std::filesystem::path(std::u8string_view((char8_t*)value.Buffer(), value.BufferSize()))
#define PATH_OF(x) std::filesystem::path(std::u8string_view((char8_t*)x.Buffer(), x.BufferSize()))


#define PATH_FUNC_STR(x) ToString(PATH.x())
#define PATH_FUNC_BOOL(x) PATH.x()
#define FILESYSTEM_FUNC_BOOL(x) \
	std::error_code ec; \
	bool out = std::filesystem::x(PATH, ec);	 \
	if (ec) \
	BLAZE_ENGINE_CORE_ERROR("std::filesystem::x returned an error: \"{}\"", ec.message());	 \
	return out; \

namespace Blaze
{
	static StringUTF8 ToString(const std::filesystem::path& path)
	{
		auto string = path.u8string();
		return StringUTF8((void*)string.data(), string.size());
	}


	StringUTF8 Path::RootName() const
	{					
		return PATH_FUNC_STR(root_name);
	}
	StringUTF8 Path::RootDirectory() const
	{
		return PATH_FUNC_STR(root_directory);
	}
	StringUTF8 Path::RootPath() const
	{
		return PATH_FUNC_STR(root_path);
	}
	StringUTF8 Path::FileName() const
	{
		return PATH_FUNC_STR(filename);
	}
	StringUTF8 Path::Stem() const
	{		
		return PATH_FUNC_STR(stem);
	}
	StringUTF8 Path::FileExtension() const
	{		
		return PATH_FUNC_STR(extension);
	}
	Path Path::ParentPath() const
	{
		return PATH_FUNC_STR(parent_path);
	}
	bool Path::IsAbsolute() const
	{
		return PATH_FUNC_BOOL(is_absolute);		
	}
	bool Path::IsDirectory() const
	{
		FILESYSTEM_FUNC_BOOL(is_directory);		
	}
	bool Path::IsFile() const
	{
		FILESYSTEM_FUNC_BOOL(is_regular_file);		
	}	
	bool Path::Exists() const
	{
		FILESYSTEM_FUNC_BOOL(exists);
	}
	uint32 Path::Hash() const
	{
		return static_cast<uint32>(std::hash<StringUTF8>{}(value));
	}
	Path Path::operator/(const Path& other) const
	{
		Path out = *this;
		out.operator/=(other);		
		return out;
	}
	Path& Path::operator/=(const Path& other)
	{
		auto path = PATH;
		path /= PATH_OF(other.value);
		value = ToString(path);
		return *this;
	}	
	bool Path::operator==(const Path& other) const
	{
		return value == other.value;
	}
	bool Path::operator!=(const Path& other) const
	{
		return value != other.value;
	}
	Path& Path::operator=(const String& string)
	{
		value = string;
		return *this;		
	}
	Path& Path::operator=(const StringView& string)
	{
		value = string;
		return *this;
	}
	Path& Path::operator=(const StringUTF8& string)
	{
		value = string;
		return *this;
	}
	Path& Path::operator=(const StringViewUTF8& string) 
	{
		value = string;
		return *this;
	}
}