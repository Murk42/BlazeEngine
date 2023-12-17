#include "pch.h"
#include "BlazeEngineCore/File/Path.h"

namespace Blaze
{
	String Path::RootName() const
	{
		if (value.Empty()) return String();
		return (const char*)std::filesystem::path(value.Ptr()).root_name().u8string().c_str();		
	}
	String Path::RootDirectory() const
	{
		if (value.Empty()) return String();
		return (const char*)std::filesystem::path(value.Ptr()).root_directory().u8string().c_str();
	}
	String Path::RootPath() const
	{
		if (value.Empty()) return String();
		return (const char*)std::filesystem::path(value.Ptr()).root_path().u8string().c_str();
	}
	String Path::FileName() const
	{
		if (value.Empty()) return String();
		return (const char*)std::filesystem::path(value.Ptr()).filename().u8string().c_str();
	}
	String Path::Stem() const
	{
		if (value.Empty()) return String();
		return (const char*)std::filesystem::path(value.Ptr()).stem().u8string().c_str();
	}
	String Path::FileExtension() const
	{
		if (value.Empty()) return String();
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
		return value.Count() == 0;
	}
}