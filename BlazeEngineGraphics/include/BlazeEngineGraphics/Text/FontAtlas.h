#pragma once
#include "BlazeEngineCore/Math/Vector.h"
#include "BlazeEngineCore/DataStructures/Map.h"
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/Textures/OpenGLTexture2D.h"

namespace Blaze
{
	class Font;
	class CharacterSet;
}
namespace Blaze::Graphics
{
	class FontAtlas
	{
	public:
		FontAtlas();
		FontAtlas(FontAtlas&& other) noexcept;
		FontAtlas(Font& fontMetrics, uint fontPixelHeight, const CharacterSet& characterSet);

		/*
			Retrieves characters UV coordinates in the font atlas

			\param character   the character whose UV coordinates to return
			\param uv1         this parameter will be filled wit the lower left point of the character rectangle 
			in the font atlas
			\param uv2         this parameter will be filled wit the top right point of the character rectangle
			in the font atlas
			\returns True if there is the requested character in the atlas, false otherwise
		*/
		bool GetCharacterUV(UnicodeChar character, Vec2f& uv1, Vec2f& uv2) const;
		inline const auto& GetTexture() const { return texture; }

		FontAtlas& operator=(FontAtlas&& other) noexcept;
	private:
		struct UVRect
		{
			Vec2f uv1;
			Vec2f uv2;
		};

		OpenGLWrapper::Texture2D texture;
		Map<UnicodeChar, UVRect> uvs;
	};
}