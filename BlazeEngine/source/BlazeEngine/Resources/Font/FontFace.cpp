#include "pch.h"
#include "BlazeEngine/Resources/Font/FontFace.h"
#include "BlazeEngine/Internal/Libraries/FreeType.h"
#include "BlazeEngineCore/File/File.h"

namespace Blaze
{	
	FontFace::FontFace()
		: ptr(nullptr)
	{
	}
	FontFace::FontFace(FontFace&& other) noexcept
		: ptr(other.ptr)
	{
		other.ptr = nullptr;
	}
	FontFace::FontFace(void* handle)
		: ptr(handle)
	{
	}
	FontFace::FontFace(const Path& path, uintMem faceIndex)
	{
		Load(path, faceIndex);
	}

	FontFace::~FontFace()
	{
		Clear();
	}
	void FontFace::Clear()
	{
		if (ptr != nullptr)
		{
			FT_Done_Face((FT_Face)ptr);

			ptr = nullptr;			
		}
	}
	bool FontFace::Load(const Path& path, uintMem faceIndex)
	{
		Clear();

		if (auto error = FT_New_Face(GetFreeTypeLibrary(), (const char*)path.GetString().Buffer(), faceIndex, (FT_Face*)&ptr))
		{
			BLAZE_ENGINE_ERROR("Failed to open font face of a font with path \"{}\" and index {}, returned FreeType error code {}", path, faceIndex, error);
			ptr = nullptr;
		}		

		if (ptr == nullptr)
			return false;

		return true;
	}
	bool FontFace::IsValidFontFace() const
	{
		return ptr != nullptr;
	}
	bool FontFace::IsItalic() const
	{
		return ptr != nullptr && bool(((FT_Face)ptr)->style_flags & FT_STYLE_FLAG_ITALIC);		
	}
	bool FontFace::IsBold() const
	{
		return ptr != nullptr && bool(((FT_Face)ptr)->style_flags & FT_STYLE_FLAG_BOLD);
	}
	FontFaceDirection FontFace::GetDirection() const
	{
		return FontFaceDirection::Horizontal;
	}
	Array<uint32> FontFace::GetAllGlyphsInidices() const
	{
		if (ptr == nullptr)
			return { };

		FT_Face face = (FT_Face)ptr;

		uint32 glyphIndex = 0;
		uint i = FT_Get_First_Char(face, &glyphIndex);		

		Array<uint32> arr;

		while (glyphIndex != 0)
		{
			arr.AddBack(glyphIndex);

			i = FT_Get_Next_Char(face, i, &glyphIndex);
		}

		return arr;
	}

	void* FontFace::ReleaseHandleOwnership()
	{
		void* oldPtr = ptr;
		ptr = nullptr;

		return oldPtr;
	}
	void* FontFace::GetHandle() const
	{
		return ptr;
	}
	FontFace& FontFace::operator=(FontFace&& other) noexcept
	{
		ptr = other.ptr;
		other.ptr = nullptr;

		return *this;
	}
}