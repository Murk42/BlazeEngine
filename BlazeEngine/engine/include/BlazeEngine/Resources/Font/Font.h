#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/DataStructures/StringViewUTF8.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/Buffer.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"
#include "BlazeEngine/Application/ResourceSystem/Resource.h"
#include <vector>
#include <unordered_map>

namespace Blaze
{
	class Font;

	enum class FontResolutionRenderType
	{
		Monochrome,
		Antialiased,
		HorizontalLCD,
		VerticalLCD,
		SDF,
	};

	struct CharacterMetrics
	{
		Vec2f uv1;
		Vec2f uv2;

		float advance;
		Vec2f renderOffset;
		Vec2f size;

		Vec2i bitmapSize;
	};

	class BLAZE_API FontResolution
	{
		Font* font;
		Graphics::Core::Texture2D atlas;
		FontResolutionRenderType renderType;
		std::unordered_map<uint, CharacterMetrics> characterMap;

		uint resolution;
		uint baselineDistance;

		uint characterCount;
	public:
		FontResolution();

		Result LoadCharacters(uint32 firsrt, uint32 last);
		Result LoadAllCharacters();

		Result CreateAtlas();

		CharacterMetrics GetCharacterData(UnicodeChar ch);

		Vec2f GetKerning(UnicodeChar left, UnicodeChar right);

		inline uint GetResolution() const { return resolution; }
		inline FontResolutionRenderType GetRenderType() const { return renderType; }
		inline Graphics::Core::Texture2D& GetAtlas() { return atlas; }
		inline const Graphics::Core::Texture2D& GetAtlas() const { return atlas; }
		inline const Font* GetFont() const { return font; }
		inline uint GetBaselineDistance() const { return baselineDistance; }

		friend class Font;
	};

	class BLAZE_API Font : public ResourceSystem::Resource
	{
		void* ptr;

		std::vector<FontResolution*> resolutions;		

		uint pixelsPerUnit; //How many pixels fit in one font unit
		float baselineDistance;

	public:
		Font();
		~Font();

		/// <summary>					Load a font file. </summary>
		/// <param name='path'>			- Path to the file </param>
		/// <param name='characters'>	- All characters to be loaded. If empty loads all characters from the file </param>		
		Result Load(StringView path);

		FontResolution* CreateFontResolution(uint resolution, FontResolutionRenderType renderType);
		inline const std::vector<FontResolution*>& GetResolutions() const { return resolutions; }		
		FontResolution* GetClosestResolution(float res) const;

		static constexpr const char* typeName = "Font";

		friend class FontResolution;
	};
}