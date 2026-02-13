#include "pch.h"
#include "BlazeEngine/Core/File/Path.h"
#include "BlazeEngine/Core/String/String.h"
#include "BlazeEngine/Core/Debug/Logger.h"
#include <filesystem>

#define PATH std::filesystem::path(std::u8string_view((char8_t*)value.Ptr(), value.Count()))
#define PATH_OF(x) std::filesystem::path(std::u8string_view((char8_t*)x.Ptr(), x.Count()))


#define PATH_FUNC_STR(x) ToString(PATH.x())
#define PATH_FUNC_BOOL(x) PATH.x()
#define FILESYSTEM_FUNC_BOOL(x) \
	std::error_code ec; \
	bool out = std::filesystem::x(PATH, ec);	 \
	if (ec) \
	BLAZE_LOG_ERROR("std::filesystem::x returned an error: \"{}\"", StringView(ec.message().data(), ec.message().size()));	 \
	return out; \


namespace Blaze
{
	static u8String ToString(const ::std::filesystem::path& path)
	{
		auto string = path.u8string();
		return u8String(string.data(), string.size());
	}

	uint64 Path::Hash() const
	{
		return value.Hash();
	}
	u8String Path::RootName() const
	{
		return PATH_FUNC_STR(root_name);
	}
	u8String Path::RootDirectory() const
	{
		return PATH_FUNC_STR(root_directory);
	}
	u8String Path::RootPath() const
	{
		return PATH_FUNC_STR(root_path);
	}
	u8String Path::FileName() const
	{
		return PATH_FUNC_STR(filename);
	}
	u8String Path::Stem() const
	{
		return PATH_FUNC_STR(stem);
	}
	u8String Path::FileExtension() const
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
	u8StringView Path::Format() const
	{
		return value;
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
	Path& Path::operator=(const u8String& string)
	{
		value = string;
		return *this;
	}
	Path& Path::operator=(const u8StringView& string)
	{
		value = string;
		return *this;
	}
	uint64 Hash<Path>::Compute(const Path& key) const
	{
		return key.GetString().Hash();
	}
}