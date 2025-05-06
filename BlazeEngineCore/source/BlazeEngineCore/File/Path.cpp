#include "pch.h"
#include "BlazeEngineCore/File/Path.h"

namespace Blaze
{
	Path::Path()
	{
	}
	Path::Path(const Path& path)
		: path(path.path)
	{ 
	}
	Path::Path(Path&& path) noexcept
		: path(std::move(path.path))
	{
	}	
	Path::Path(const StringViewUTF8& string)
		: path((const char8_t*)string.Buffer(), (const char8_t*)string.Buffer() + string.BufferSize() - 1)
	{
	}
	StringUTF8 Path::ToString() const
	{
		auto string = path.u8string();
		return StringUTF8(string.c_str(), string.size());
	}
	StringUTF8 Path::RootName() const
	{		
		auto u8 = path.root_name().u8string();
		return StringUTF8((void*)u8.data(), u8.size());
	}
	StringUTF8 Path::RootDirectory() const
	{
		auto u8 = path.root_directory().u8string();
		return StringUTF8((void*)u8.data(), u8.size());
	}
	StringUTF8 Path::RootPath() const
	{
		auto u8 = path.root_path().u8string();
		return StringUTF8((void*)u8.data(), u8.size());
	}
	StringUTF8 Path::FileName() const
	{
		auto u8 = path.filename().u8string();
		return StringUTF8((void*)u8.data(), u8.size());
	}
	StringUTF8 Path::Stem() const
	{
		auto u8 = path.stem().u8string();
		return StringUTF8((void*)u8.data(), u8.size());
	}
	StringUTF8 Path::FileExtension() const
	{
		auto u8 = path.extension().u8string();
		return StringUTF8((void*)u8.data(), u8.size());
	}
	Path Path::ParentPath() const
	{
		return Path(path.parent_path());
	}
	bool Path::IsAbsolute() const
	{
		return path.is_absolute();
	}
	bool Path::IsDirectory() const
	{
		std::error_code ec;
		bool value = std::filesystem::is_directory(path, ec);

		if (ec)
		{
			auto message = ec.message();
			BLAZE_ENGINE_CORE_ERROR("std::filesystem::exists returned an error: \"" + StringView(message.data(), message.c_str()) + "\"");
		}

		return value;
	}
	bool Path::IsFile() const
	{
		std::error_code ec;
		bool value = std::filesystem::is_regular_file(path, ec);

		if (ec)
		{
			auto message = ec.message();
			BLAZE_ENGINE_CORE_ERROR("std::filesystem::exists returned an error: \"" + StringView(message.data(), message.c_str()) + "\"");
		}

		return value;		
	}
	bool Path::Empty() const
	{
		return path.empty();
	}
	bool Path::Exists() const
	{
		std::error_code ec;
		bool value = std::filesystem::exists(path, ec);

		if (ec)
		{			
			auto message = ec.message();
			BLAZE_ENGINE_CORE_ERROR("std::filesystem::exists returned an error: \"" + StringView(message.data(), message.c_str()) + "\"");
		}

		return value;
	}
	uint32 Path::Hash() const
	{
		return static_cast<uint32>(std::hash<std::filesystem::path>{}(path));
	}
	Path Path::operator/(const Path& other) const
	{
		Path out{ path };
		out.path.operator/=(other.path);		
		return out;
	}
	Path Path::operator/(const StringView& other) const
	{
		Path out{ path };
		out.path.append(other.Ptr(), other.Ptr() + other.Count());
		return out;
	}
	Path Path::operator/(const StringViewUTF8& other) const
	{
		Path out{ path };
		out.path.append((char8_t*)other.Buffer(), (char8_t*)other.Buffer() + other.BufferSize() - 1);
		return out;
	}

	bool Path::operator==(const Path& other) const
	{
		return path == other.path;
	}
	bool Path::operator!=(const Path& other) const
	{
		return path != other.path;
	}

	Path& Path::operator=(const Path& other)
	{
		path = other.path;
		return *this;
	}
	Path& Path::operator=(Path&& other) noexcept
	{
		path = std::move(other.path);
		return *this;
	}
	Path& Path::operator=(const StringView& string)
	{
		path = std::filesystem::path(string.Ptr(), string.Ptr() + string.Count());
		return *this;
	}
	Path& Path::operator=(const StringViewUTF8& string) 
	{
		if (string.Empty())
			path.clear();
		else
			path = std::filesystem::path((char8_t*)string.Buffer(), (char8_t*)string.Buffer() + string.BufferSize() - 1);
		return *this;
	}

	Path::Path(const std::filesystem::path& path)
		: path(path)
	{
	}

	std::filesystem::path& Path::GetUnderlyingObject()
	{
		return path;
	}

	const std::filesystem::path& Path::GetUnderlyingObject() const
	{
		return path;
	}

}