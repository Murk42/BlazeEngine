#pragma once
#include "BlazeEngine/Core/Resource/ResourceRef.h"
#include "BlazeEngine/Core/File/Path.h"

namespace Blaze::UI
{
	class File;

	enum class FontFaceDirection
	{
		Horizontal,
		Vertical
	};

	class BLAZE_API FontFace
	{
	public:
		FontFace();
		FontFace(FontFace&& other) noexcept;
		FontFace(void* handle);
		FontFace(const Path& path, uintMem faceIndex);
		~FontFace();

		void Clear();

		bool Load(const Path& path, uintMem faceIndex);

		bool IsValidFontFace() const;
		bool IsItalic() const;
		bool IsBold() const;
		FontFaceDirection GetDirection() const;

		Array<uint32> GetAllGlyphsInidices() const;

		void* ReleaseHandleOwnership();
		void* GetHandle() const;

		FontFace& operator=(FontFace&& other) noexcept;

		static FontFace LoadDefault();
	private:
		void* ptr;
	};
}