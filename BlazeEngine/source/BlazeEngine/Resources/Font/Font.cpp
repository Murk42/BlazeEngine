#include "pch.h"
#include "BlazeEngine/Resources/Font/Font.h"
#include "BlazeEngine/Internal/Libraries/FreeType.h"
#include "BlazeEngineCore/File/File.h"

namespace Blaze
{	
	Font::Font()
		: ptr(nullptr)
	{
	}
	Font::Font(Font&& other) noexcept
		: ptr(other.ptr)
	{
		other.ptr = nullptr;
	}
	Font::Font(void* handle)
		: ptr(handle)
	{
	}
	Font::Font(const Path& path)
		: Font()
	{
		Load(path);
	}
	Font::~Font()
	{
		Clear();
	}	
	void Font::Clear()
	{
		if (ptr != nullptr)
		{
			if (auto error = FT_Done_Face((FT_Face)ptr))			
				BLAZE_ENGINE_ERROR("Failed to release font successfully with FreeType error code {}", error);

			ptr = nullptr;
		}
	}	
	bool Font::Load(const Path& path)
	{
		Clear();

		if (auto error = FT_New_Face(GetFreeTypeLibrary(), (const char*)path.GetString().Buffer(), -1, (FT_Face*)&ptr))
		{
			BLAZE_ENGINE_ERROR("Failed to open font with path \"{}\", returned FreeType error code {}", path, error);
			ptr = nullptr;
		}

		if (ptr == nullptr)
			return false;		

		return true;
	}
	bool Font::IsValidFont() const
	{
		return ptr != nullptr;
	}
	uintMem Font::GetFaceCount() const
	{
		if (ptr == nullptr)
			return 0;

		return ((FT_Face)ptr)->num_faces;
	}	
	void* Font::ReleaseHandleOwnership()
	{
		void* oldPtr = ptr;
		ptr = nullptr;

		return oldPtr;
	}
	void* Font::GetHandle() const
	{
		return ptr;
	}
	Font& Font::operator=(Font&& other) noexcept
	{
		ptr = other.ptr;
		other.ptr = nullptr;

		return *this;
	}
}