#include "BlazeEngine/File/Path.h"
#include <filesystem>

namespace Blaze
{
	String Path::RootName() const
	{
		return (const char*)std::filesystem::path(value.Ptr()).root_name().u8string().c_str();		
	}
	String Path::RootDirectory() const
	{
		return (const char*)std::filesystem::path(value.Ptr()).root_directory().u8string().c_str();
	}
	String Path::RootPath() const
	{
		return (const char*)std::filesystem::path(value.Ptr()).root_path().u8string().c_str();
	}
	String Path::FileName() const
	{
		return (const char*)std::filesystem::path(value.Ptr()).filename().u8string().c_str();
	}
	String Path::Stem() const
	{
		return (const char*)std::filesystem::path(value.Ptr()).stem().u8string().c_str();
	}
	String Path::FileExtension() const
	{
		return (const char*)std::filesystem::path(value.Ptr()).extension().u8string().c_str();
	}
	bool Path::IsAbsolute() const
	{
		return std::filesystem::path(value.Ptr()).is_absolute();
	}
	bool Path::IsDirectory() const
	{
		return std::filesystem::is_directory(std::filesystem::path(value.Ptr()));
	}
	bool Path::IsFile() const
	{
		return std::filesystem::is_regular_file(std::filesystem::path(value.Ptr()));
	}
	bool Path::IsEmpty() const
	{
		return value.Size() == 0;
	}
}