#pragma once
#include "BlazeEngine/Core/Resource/ResourceRef.h"

namespace Blaze::UI
{

	class BLAZE_API Font
	{
	public:
		/*
			Constructs a empty font that is considered invalid
		*/
		Font();
		/*
			Moves an another font to this one
		*/
		Font(Font&& other) noexcept;
		/*
			Constructs a new font object by taking ownership of the new internal handle and by referencing its
			data source

			\param handle - the internal object handle
		*/
		Font(void* handle);
		/*
			Loads the font from afile. If everything is read successfully the font is
			considered valid

			\param path - the path to the file
		*/
		Font(const Path& path);
		~Font();

		/*
			Clears the font. After this the font is empty and is considered invalid
		*/
		void Clear();

		/*
			Loads the font by reading data from the file. If everything is read successfully the font is
			considered valid. The old font data is cleared

			\param file - the file that stores the font data
		*/
		bool Load(const Path& path);

		/*
			\returns True if the font is valid, false otherwise
		*/
		bool IsValidFont() const;
		/*
			\returns The number of faces in the font. Zero is returned if the font isn't valid
		*/
		uintMem GetFaceCount() const;

		/*
			Disowns the handle and returns it. The font wont be referencing its source anymore

			\returns The handle to its internal object
		*/
		void* ReleaseHandleOwnership();
		/*
			\returns The handle to the internal object
		*/
		void* GetHandle() const;

		/*
			Moves an another font to this one
		*/
		Font& operator=(Font&& other) noexcept;
	private:
		void* ptr;
	};
}