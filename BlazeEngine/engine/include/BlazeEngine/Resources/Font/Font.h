#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/DataStructures/StringViewUTF8.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/Buffer.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"
#include <vector>
#include <unordered_map>

namespace Blaze
{		
	class Font;

	struct CharacterData
	{
		float advance;
		Vec2f renderOffset;
		Vec2f size;

		Vec2f uv1;
		Vec2f uv2;
	};

	struct CharacterMetrics
	{		
		float advance;
		Vec2f renderOffset;
		Vec2f size;
	};
		               
	class BLAZE_API FontResolution
	{		
		struct CharacterUV
		{
			Vec2f uv1;
			Vec2f uv2;

			CharacterMetrics* metrics;
		};

		Font* font;
		uint resolution;
		Graphics::Core::Texture2D atlas;
		std::unordered_map<uint, CharacterUV> characterMap;
		float baselineDistance;
		
		void LoadAtlas();
	public:

		CharacterData GetCharacterData(UnicodeChar ch);
		uint GetResolution() const { return resolution; }		
		Graphics::Core::Texture2D& GetAtlas() { return atlas; }
		Font* GetFont() { return font; }
		const Font* GetFont() const { return font; }
		inline float GetBaselineDistance() const { return baselineDistance; }

		friend class Font;
	};

	class BLAZE_API Font
	{		
		void* ptr;					

		std::vector<FontResolution*> resolutions;				
		std::unordered_map<uint, CharacterMetrics> characterMap;				
		uint characterCount;
		uint pixelsPerUnit; //How many pixels fit in one font unit

		void LoadFontMetrics();
	public:
		Font();		
		~Font();				
					
		int Load(StringView path, bool emitLogOnFail = true);
		FontResolution* CreateFontResolution(uint resolution);				
						
		inline std::vector<FontResolution*> GetResolutions() const { return resolutions; }

		friend class FontResolution;
	};	
}